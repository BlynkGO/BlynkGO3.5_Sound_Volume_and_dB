#include <BlynkGOv2.h>

IMG_DECLARE(img_background);
IMG_DECLARE(img_icon_audiolab1);
IMG_DECLARE(img_icon_audiolab2);
IMG_DECLARE(img_icon_audiolab3);
IMG_DECLARE(img_audiolab_gauge_vertical);
IMG_DECLARE(img_audiolab_gauge_double);
IMG_DECLARE(img_gauge_hand);
IMG_DECLARE(img_spectrum_bar);

typedef struct _sound_vol_t {
  //-------------------------------------------
  typedef struct _bar_spectrum_t {
    GImage bg; GRect  rect; float _v;
    void design(GWidget &w){
      bg = img_spectrum_bar; bg.parent(w); 
      rect.size(bg); rect.color(TFT_BLACK); rect.parent(bg); rect.align(ALIGN_TOP);
      bg.clickable(false); rect.clickable(false);
    }
    void value(float v){
      _v = constrain(v,0,1000); 
      rect.height(map_f( v, 0, 1000, bg.height(),0)); rect.align(ALIGN_TOP);
    }
    float value() { return _v; }
  } bar_spectrum_t;
  //-------------------------------------------
  GContainer cont;
    bar_spectrum_t bar[8];
  //-------------------------------------------
  bool _is_designed =false;
  void design(GWidget &widget= GScreen){
    cont.opa(0);
    cont.layout(LAYOUT_ROW_B);
    cont.padding(0,0,0,0,10);
    cont.clickable(false);
    for(int i=0;i<8;i++){
      bar[i].design(cont);
    }
    cont.parent(widget);
    _is_designed = true;
  }
  void value(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8){
    if(!_is_designed) this->design();
    bar[0].value(f1); bar[1].value(f2); bar[2].value(f3); bar[3].value(f4);
    bar[4].value(f5); bar[5].value(f6); bar[6].value(f7); bar[7].value(f8);
  }
  void hidden(bool enable)    { cont.hidden(enable);  }
  bool hidden()               { return cont.hidden(); }
  void parent(GWidget&widget) { cont.parent(widget);  }
  //-------------------------------------------
} sound_volume_t;

GImage background(img_background);
GContainer cont_btn;
  GImageButton btn_audiolab1(img_icon_audiolab1, cont_btn);
  GImageButton btn_audiolab2(img_icon_audiolab2, cont_btn);
  GImageButton btn_audiolab3(img_icon_audiolab3, cont_btn);

GRect rect_audiolab1;
  GImage audiolab1(img_audiolab_gauge_vertical, rect_audiolab1);

  #define DB_LEFT      true
  #define DB_RIGHT     false
  typedef struct _gauge_line_t {
    GContainer cont;
    GRect  rect[53];
    GRect  rect_dB;
    bool   _is_left_dB;
    void design(GWidget&widget, bool is_left_db){
      _is_left_dB = is_left_db;
      cont.opa(0);
      cont.layout(LAYOUT_ROW_M);
      cont.padding(0,0,0,0,3);
      cont.parent(widget);
      cont.clickable(false);
      for(int i=0; i< 53; i++){
        rect[i].size(4,32);
        rect[i].clickable(false);
        rect[i].parent(cont);
        rect[i].color(TFT_COLOR_HSV(  map(i,0,52, 152 , 12 ),
                                      map(i,0,52,  98 , 79 ),
                                      map(i,0,52,  61 , 80 ) ));
      }
      rect_dB.parent(widget);
      rect_dB.size(370,32); 
      rect_dB.clickable(false);
      rect_dB.color(TFT_COLOR_HEX(0x030303));
      rect_dB.align(_is_left_dB? ALIGN_TOP_RIGHT : ALIGN_BOTTOM_RIGHT,-22,0);
      cont.align   (_is_left_dB? ALIGN_TOP_LEFT  : ALIGN_BOTTOM_LEFT , 39,0);
    }
    void volume_dB(int8_t dB){
      dB = constrain(dB,-42,10);
      rect_dB.width( map(dB,-42,10, cont.width(), 0) );
    }
  } gauge_line_t;
  gauge_line_t gauge_line[2];
  
GRect rect_audiolab2;
  GImage audiolab2(img_audiolab_gauge_double, rect_audiolab2);
    GImage gauge_hand_db_left(img_gauge_hand, audiolab2);
    GImage gauge_hand_db_right(img_gauge_hand, audiolab2);

GRect rect_sound_volume;
  sound_volume_t sound_volume;

void setup() {
  Serial.begin(115200); Serial.println();
  BlynkGO.begin();

  background.position(0,0);
  cont_btn.opa(0);
  cont_btn.layout(LAYOUT_ROW_M);
  cont_btn.padding(0,0,0,0,20);
    btn_audiolab1.onClicked([](GWidget*widget){
      rect_audiolab1.hidden(false);
    });
    btn_audiolab2.onClicked([](GWidget*widget){
      rect_audiolab2.hidden(false);
    });
    btn_audiolab3.onClicked([](GWidget*widget){
      rect_sound_volume.hidden(false);
    });
  cont_btn.align(ALIGN_BOTTOM,0,-20);

  rect_audiolab1.hidden(true);
  rect_audiolab1.color(TFT_COLOR_HEX(0x030303));
  rect_audiolab1.onClicked([](GWidget*widget){
    rect_audiolab1.hidden(true);
  });
    audiolab1.align_center();
    gauge_line[0].design(audiolab1, DB_LEFT);
    gauge_line[1].design(audiolab1, DB_RIGHT);

  rect_audiolab2.hidden(true);
  rect_audiolab2.color(TFT_COLOR_HEX(0x030303));
  rect_audiolab2.onClicked([](GWidget*widget){
    rect_audiolab2.hidden(true);
  });
    audiolab2.align_center();
      gauge_hand_db_left.center(126,179);
      gauge_hand_db_right.center(366,179);  //x +240    

      gauge_hand_db_left.angle(-50);
      gauge_hand_db_right.angle(-50);
  
  rect_sound_volume.hidden(true);
  rect_sound_volume.color(TFT_BLACK);
  rect_sound_volume.onClicked([](GWidget*widget){
    rect_sound_volume.hidden(true);
  });
    sound_volume.design(rect_sound_volume);

    static GTimer timer;
    timer.setInterval(100,[](){
      int8_t dB = random(-42,10);
      gauge_line[0].volume_dB(dB);
      gauge_line[1].volume_dB(dB);
      gauge_hand_db_left .angle( map(dB,-42,10,-50,50) );
      gauge_hand_db_right.angle( map(dB,-42,10,-50,50) );
      
      sound_volume.value( random(1000),random(1000),random(1000),random(1000),
                          random(1000),random(1000),random(1000),random(1000));    
    });
}

void loop() {
  BlynkGO.update();
}
