$stipples = {} unless $stipples

$stipples['bdiagonal1'] = Tk::BLT::Bitmap.new(<<EOD)
#define bdiagonal1_width 8
#define bdiagonal1_height 8
static unsigned char bdiagonal1_bits[] = {
   0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11};
EOD

$stipples['bdiagonal2'] = Tk::BLT::Bitmap.new(<<EOD)
#define bdiagonal2_width 8
#define bdiagonal2_height 8
static unsigned char bdiagonal2_bits[] = {
   0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20, 0x10};
EOD

$stipples['checker2'] = Tk::BLT::Bitmap.new(<<EOD)
#define checker2_width 8
#define checker2_height 8
static unsigned char checker2_bits[] = {
   0x33, 0x33, 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc};
EOD


$stipples['checker3'] = Tk::BLT::Bitmap.new(<<EOD)
#define checker3_width 8
#define checker3_height 8
static unsigned char checker3_bits[] = {
   0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0};
EOD

$stipples['cross1'] = Tk::BLT::Bitmap.new(<<EOD)
#define cross1_width 8
#define cross1_height 8
static unsigned char cross_bits[] = {
   0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa};
EOD

$stipples['cross2'] = Tk::BLT::Bitmap.new(<<EOD)
#define cross2_width 8
#define cross2_height 8
static unsigned char cross2_bits[] = {
   0xff, 0x88, 0x88, 0x88, 0xff, 0x88, 0x88, 0x88};
EOD

$stipples['cross3'] = Tk::BLT::Bitmap.new(<<EOD)
#define cross3_width 8
#define cross3_height 8
static unsigned char cross3_bits[] = {
   0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
EOD

$stipples['crossdiag'] = Tk::BLT::Bitmap.new(<<EOD)
#define crossdiag_width 8
#define crossdiag_height 8
static unsigned char crossdiag2_bits[] = {
   0x18, 0x24, 0x42, 0x81, 0x81, 0x42, 0x24, 0x18};
EOD

$stipples['dot1'] = Tk::BLT::Bitmap.new(<<EOD)
#define dot1_width 8
#define dot1_height 8
static unsigned char dot1_bits[] = {
   0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
EOD

$stipples['dot2'] = Tk::BLT::Bitmap.new(<<EOD)
#define dot2_width 8
#define dot2_height 8
static unsigned char dot2_bits[] = {
   0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00};
EOD

$stipples['dot3'] = Tk::BLT::Bitmap.new(<<EOD)
#define dot3_width 8
#define dot3_height 8
static unsigned char dot3_bits[] = {
   0x11, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00};
EOD

$stipples['dot4'] = Tk::BLT::Bitmap.new(<<EOD)
#define dot4_width 8
#define dot4_height 8
static unsigned char dot4_bits[] = {
   0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
EOD

$stipples['fdiagonal1'] = Tk::BLT::Bitmap.new(<<EOD)
#define fdiagonal1_width 8
#define fdiagonal1_height 8
static unsigned char fdiagonal1_bits[] = {
   0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88};
EOD

$stipples['fdiagonal2'] = Tk::BLT::Bitmap.new(<<EOD)
#define fdiagonal2_width 8
#define fdiagonal2_height 8
static unsigned char fdiagonal2_bits[] = {
   0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08};
EOD

$stipples['hline1'] = Tk::BLT::Bitmap.new(<<EOD)
#define hline1_width 8
#define hline1_height 8
static unsigned char hline1_bits[] = {
   0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
EOD

$stipples['hline2'] = Tk::BLT::Bitmap.new(<<EOD)
#define hline2_width 8
#define hline2_height 8
static unsigned char hline2_bits[] = {
   0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00};
EOD

$stipples['lbottom'] = Tk::BLT::Bitmap.new(<<EOD)
#define lbottom_width 8
#define lbottom_height 8
static unsigned char lbottom_bits[] = {
   0x00, 0x11, 0x11, 0x77, 0x00, 0x11, 0x11, 0x77};
EOD

$stipples['ltop'] = Tk::BLT::Bitmap.new(<<EOD)
#define ltop_width 8
#define ltop_height 8
static unsigned char ltop_bits[] = {
   0xee, 0x88, 0x88, 0x00, 0xee, 0x88, 0x88, 0x00};
EOD

$stipples['rbottom'] = Tk::BLT::Bitmap.new(<<EOD)
#define rbottom_width 8
#define rbottom_height 8
static unsigned char rbottom_bits[] = {
   0x00, 0x88, 0x88, 0xee, 0x00, 0x88, 0x88, 0xee};
EOD

$stipples['rtop'] = Tk::BLT::Bitmap.new(<<EOD)
#define rtop_width 8
#define rtop_height 8
static unsigned char rtop_bits[] = {
   0x77, 0x11, 0x11, 0x00, 0x77, 0x11, 0x11, 0x00};
EOD

$stipples['vline1'] = Tk::BLT::Bitmap.new(<<EOD)
#define vline1_width 8
#define vline1_height 8
static unsigned char vline1_bits[] = {
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
EOD

$stipples['vline2'] = Tk::BLT::Bitmap.new(<<EOD)
#define vline2_width 8
#define vline2_height 8
static unsigned char vline2_bits[] = {
   0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
EOD
