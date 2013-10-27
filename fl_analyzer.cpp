#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <remocon.h>

class Histogram:public Fl_Widget{
	const char *t;
	char buf[32];
	long *s;
	int c;
	long center;
	void draw(){
		int i;
		int pitch, level;

		fl_color(FL_BLACK);
		fl_rectf(x(), y(), w(), h());
		if(c <= 0){
			return;
		}
		pitch = w() / c;

		fl_color(FL_GREEN);
		for(i = 0; i < c; i++){
			level = (center - s[i]) / 2 + h() / 2 + y();
			fl_rectf(
				i * pitch + x(),
				level,
				pitch > 1 ? pitch - 1 : pitch,
				h() + y() - level);
		}
		fl_color(FL_WHITE);
		fl_line(x(), h() / 2 + y(), w() + x(), h() / 2 + y());
		fl_font(FL_HELVETICA, 10);
		fl_draw(buf, x(), y() + 10);
	}
public:
	Histogram(int X, int Y, int W, int H, const char *title):Fl_Widget(X, Y, W, H, NULL){
		t = title;
		s = NULL;
		c = 0;
	}

	long setdata(int count, long *sig){
		int i;

		free(s);
		c = count;
		s = sig;
		center = rc_center(s, c);
		sprintf(buf, "%s=%dusec(%dsamples)", t, center, c);
		redraw();
		return center;
	}
};

class Waveform:public Fl_Widget{
	const char *t;
	char buf[32];
	long sh, sl;
	void draw(){
		int wt, wb, wc;
		wt = y() + 24;// wave top
		wb = y() + h() - 10;// wave bottom
		wc = (w() - 20) * sh / (sh + sl) + x() + 10;// wave change

		fl_color(FL_BLACK);
		fl_rectf(x(), y(), w(), h());
		if(sh == 0 && sl == 0){
			return;
		}
		fl_color(FL_GREEN);
		fl_xyline(x(), wb, x() + 10);
		fl_yxline(x() + 10, wt, wb);
		fl_xyline(x() + 10, wt, wc);
		fl_yxline(wc, wt, wb);
		fl_xyline(wc, wb, x() + w() - 10);
		fl_yxline(x() + w() - 10, wt, wb);
		fl_xyline(x() + w() - 10, wt, x() + w());
		
		fl_color(FL_WHITE);
		fl_font(FL_HELVETICA, 10);
		fl_draw(t, x(), y() + 10);
		sprintf(buf, "%dusec", sh);
		fl_draw(buf, x() + 12, wt - 2);
		sprintf(buf, "%dusec", sl);
		fl_draw(buf, wc + 2, wb - 2);
	}
public:
	Waveform(int X, int Y, int W, int H, const char *title):Fl_Widget(X, Y, W, H, NULL){
		t = title;
		sh = sl = 0;
	}

	void setdata(long high, long low){
		sh = high;
		sl = low;
		redraw();
	}
};

struct allspec{
	int new_data;
	int memo_data;
	int not_saved;
	int serial;
	opspec op;
	irspec ir;
	rcspec rc;
	Histogram *dist_t;
	Histogram *dist_1;
	Histogram *dist_0;
	Waveform *wave_l;
	Waveform *wave_1;
	Waveform *wave_0;
	Fl_Text_Display *report;
	Fl_Text_Buffer *report_buf;
	Fl_Button *btn_rec;
	Fl_Button *btn_play;
	Fl_Button *btn_memo;
	Fl_Button *btn_save;
	Fl_Button *btn_exit;
};

void cb_rec(Fl_Widget* widget, void* pall){
	int i;
	char buf[80];
	int ret;
	int length;
	int index;
	long *work;
	allspec *all;

	all = (allspec *)pall;
	all->ir.fd = rc_passign(all->op.port);
	if(all->ir.fd <= 0){
		close(all->ir.fd);
		rc_pfree(all->op.port);
		all->report_buf->text("Port invalid.\nRestoration carried out.\nTry again.");
		return;
	}

	ret = rc_start(&all->ir);
	if(ret <= 0){
		all->report_buf->text("Illegal signal format.\nTry again.");
		close(all->ir.fd);
		rc_pfree(all->op.port);
		return;
	}

	rc_record(&all->ir);
	length = all->ir.length;

	ret = rc_next(&all->ir);
	if(ret <= 0){
		all->rc.multi = 0;
	} else
	if((all->ir.repeat < all->ir.loff * 8 / 10) ||
	(all->ir.repeat > all->ir.loff * 12 / 10)){
		fprintf(stderr, "WARNNING: Repeat code was found. 2nd signal was reducd.\n"
		"Leader off: %dusec, standard: %dusec.\n", all->ir.repeat, all->ir.loff);
		all->rc.multi = 0;
	} else {
		all->ir.code = all->rc.code2;
		rc_decode(&all->ir);
		all->rc.multi = 1;
	}

	close(all->ir.fd);
	rc_pfree(all->op.port);

	// setup all->rc
	all->rc.lon = all->ir.lon;
	all->rc.loff = all->ir.loff;
	for(i = 0; i < length; i++){
		all->rc.code1[i / 8] <<= 1;
		all->rc.code1[i / 8] |= (all->ir.son[i] * 2 < all->ir.soff[i]);
	}

	all->rc.count1 = (length + 7) / 8;
	if(length % 8)
		fprintf(stderr, "WARNNING: 1st shot is not filled with bit.\n");

	if(all->rc.multi){
		all->rc.interval = all->ir.interval;
		all->rc.gap = all->ir.gap;
		all->rc.count2 = all->ir.count;
		if(all->ir.length % 8)
		fprintf(stderr, "WARNNING: 2nd shot is not filled with bit.\n");
	}

	work = (long *)calloc(length, sizeof(long));
	for(i = 0; i < length; i++)
		work[i] = all->ir.son[i];
	all->rc.t = all->dist_t->setdata(length, work);

	index = 0;
	for(i = 0; i < length; i++)
		if(all->rc.t * 2 < all->ir.soff[i])
			index++;
	work = (long *)calloc(index, sizeof(long));
	index = 0;
	for(i = 0; i < length; i++)
		if(all->rc.t * 2 < all->ir.soff[i])
			work[index++] = all->ir.soff[i];
	all->rc.h = all->dist_1->setdata(index, work);

	index = 0;
	for(i = 0; i < length; i++)
		if(all->rc.t * 2 >= all->ir.soff[i])
			index++;
	work = (long *)calloc(index, sizeof(long));
	index = 0;
	for(i = 0; i < length; i++)
		if(all->rc.t * 2 >= all->ir.soff[i])
			work[index++] = all->ir.soff[i];
	all->rc.l = all->dist_0->setdata(index, work);

	all->wave_l->setdata(all->rc.lon, all->rc.loff);
	all->wave_1->setdata(all->rc.t, all->rc.h);
	all->wave_0->setdata(all->rc.t, all->rc.l);

	all->report_buf->text("Analyze complete as follows.\n\n");

	if(all->rc.multi)
		all->report_buf->append("1st signal\n");

	sprintf(buf, "Signal count: %dbytes.\n", all->rc.count1);
	all->report_buf->append(buf);

	for(i = 0; i < all->rc.count1 - 1; i++){
		sprintf(buf, "%02X", all->rc.code1[i]);
		all->report_buf->append(buf);
		all->report_buf->append((i + 1) % 8 ? ", " : "\n");
	}
	sprintf(buf, "%02X.\n\n", all->rc.code1[i]);
	all->report_buf->append(buf);

	if(all->rc.multi){
		sprintf(buf, "Gap: %dusec.\n", all->rc.gap);
		all->report_buf->append(buf);
		sprintf(buf, "Interval: %dusec.\n\n", all->rc.interval);
		all->report_buf->append(buf);

		sprintf(buf, "Signal count: %dbytes.\n", all->rc.count2);
		all->report_buf->append(buf);

		for(i = 0; i < all->rc.count2 - 1; i++){
			sprintf(buf, "%02X", all->rc.code2[i]);
			all->report_buf->append(buf);
			all->report_buf->append((i + 1) % 8 ? ", " : "\n");
		}
		sprintf(buf, "%02X.\n\n", all->rc.code2[i]);
		all->report_buf->append(buf);
	}
	all->new_data = 1;
	all->btn_play->activate();
	all->btn_memo->activate();
}

void cb_play(Fl_Widget* widget, void *pall){
	int ret;
	allspec *all;
	all = (allspec *)pall;

	ret = rc_shot(&all->rc);
	if(ret < 0){
		all->report_buf->text("Fail to Sot.\n");
	} else {
		all->report_buf->text("IR Sot done.\n");
	}
}

void cb_memo(Fl_Widget* widget, void *pall){
	int ret;
	char buf[8];
	allspec *all;
	all = (allspec *)pall;

	if(all->serial >= 99){
		all->report_buf->text("Too many entries.Do nothing.\n");
		return;
	}

	ret = rc_memo(all->serial, &all->rc);
	if(ret < 0){
		all->report_buf->text("Fail to Memorise.\n");
		return;
	}

	all->report_buf->text("Memorize by name of button");
	sprintf(buf, "%02d.", all->serial);
	all->report_buf->append(buf);
	all->serial++;
	all->new_data = 0;
	all->memo_data = 1;
	all->not_saved = 1;
	all->btn_memo->deactivate();
	all->btn_save->activate();
}

void cb_save(Fl_Widget* widget, void *pall){
	char buf[80];
	int ret;
	allspec *all;
	all = (allspec *)pall;

	ret = rc_save(all->serial, &all->op, &all->rc);
	if(ret < 0){
		all->report_buf->text("Fail to save.\n");
		return;
	}

	all->new_data = 0;
	all->memo_data = 0;
	all->not_saved = 0;
	all->serial = 0;
	all->btn_memo->deactivate();
	all->btn_save->deactivate();
	if(!all->op.lirc){
		sprintf(buf, "Create %s/analyzer.conf.\n", all->op.dir);
		all->report_buf->text(buf);
		all->report_buf->append("saved in fl_analyzer standard format.\n\n");
	} else {
		sprintf(buf, "Create %s/lirc.conf.\n", all->op.dir);
		all->report_buf->text(buf);
		if(all->rc.multi == 0 || all->rc.count1 == all->rc.count2){
			all->report_buf->append("saved in lirc standard format.\n\n");
		} else {
			all->report_buf->append("saved in lirc raw format.\n\n");
		}
	}
	all->report_buf->append("fl_analyzer initialized and restart.\n");
	all->report_buf->append("If you continue and resave,\nsaved data will be over written.\n");
}

void cb_exit(Fl_Widget*, void* pall){
	allspec *all;
	all = (allspec *)pall;
	int ret;

	if(all->not_saved){
		ret = fl_choice("Memorized data are not saved.\nExit really?", "Exit", "Return", NULL);
		if(ret != 0)
			return;
	}
	exit(0);
}

int main(int argc, char **argv){
	char buf[80];
	long son[RC_SIGLEN], soff[RC_SIGLEN];
	uint8_t code1[RC_SIGCNT], code2[RC_SIGCNT];
	allspec all;

	all.new_data = 0;
	all.memo_data = 0;
	all.not_saved = 0;
	all.serial = 0;

	all.ir.son = son;
	all.ir.soff = soff;
	all.rc.code1 = code1;
	all.rc.code2 = code2;

	rc_getopt(argc, argv, &all.op);
	Fl_Double_Window win(540, 232, "Fl_analyzer");
	all.dist_t = new Histogram(10, 10, 160, 64, "T");
	all.dist_1 = new Histogram(10, 84, 160, 64, "H");
	all.dist_0 = new Histogram(10, 158, 160, 64, "L");
	all.wave_l = new Waveform(180, 10, 160, 64, "Leader");
	all.wave_1 = new Waveform(180, 84, 160, 64, "Signal High");
	all.wave_0 = new Waveform(180, 158, 160, 64, "Signal Low");
	all.report = new Fl_Text_Display(350, 10, 180, 138);
	all.report_buf = new Fl_Text_Buffer();
	all.report->buffer(all.report_buf);
	all.report->textsize(10);
	all.btn_rec = new Fl_Button(350, 158, 56, 20, "@-5circle");
	all.btn_rec->callback(cb_rec, &all);
	all.btn_rec->deactivate();
	all.btn_play = new Fl_Button(412, 158, 56, 20, "@-2>");
	all.btn_play->callback(cb_play, &all);
	all.btn_play->deactivate();
	all.btn_memo = new Fl_Button(474, 158, 56, 20, "@-4+");
	all.btn_memo->callback(cb_memo, &all);
	all.btn_memo->deactivate();
	all.btn_save = new Fl_Button(350, 202, 56, 20, "Save");
	all.btn_save->labelsize(12);
	all.btn_save->callback(cb_save, &all);
	all.btn_save->deactivate();
	all.btn_exit = new Fl_Button(412, 202, 56, 20, "Exit");
	all.btn_exit->labelsize(12);
	all.btn_exit->callback(cb_exit, &all);
	win.show();
	all.btn_rec->activate();
	return(Fl::run());
}
