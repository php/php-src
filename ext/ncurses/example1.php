<?php
$n=0;
ncurses_init();
if(ncurses_has_colors()){
ncurses_start_color();
        ncurses_init_pair(1,NCURSES_COLOR_RED,NCURSES_COLOR_BLACK);
        ncurses_init_pair(2,NCURSES_COLOR_GREEN,NCURSES_COLOR_BLACK);
        ncurses_init_pair(3,NCURSES_COLOR_YELLOW,NCURSES_COLOR_BLACK);
        ncurses_init_pair(4,NCURSES_COLOR_BLUE,NCURSES_COLOR_BLACK);
        ncurses_init_pair(5,NCURSES_COLOR_MAGENTA,NCURSES_COLOR_BLACK);
        ncurses_init_pair(6,NCURSES_COLOR_CYAN,NCURSES_COLOR_BLACK);
        ncurses_init_pair(7,NCURSES_COLOR_WHITE,NCURSES_COLOR_BLACK);
}
while(1){
for ($x=0; $x<80; $x++) {
for ($y=0; $y<24; $y++) {
  $n++;
  ncurses_move($y,$x);
  ncurses_addch($n+64);
  ncurses_color_set($n%8);
  ncurses_refresh();
  if($n>26)$n=0;
}
}
ncurses_getch();
}
?>

