#define FILTER_FLAG_NONE                    0x0000

#define FILTER_FLAG_ALLOW_OCTAL             0x0001
#define FILTER_FLAG_ALLOW_HEX               0x0002

#define FILTER_FLAG_STRIP_LOW               0x0004
#define FILTER_FLAG_STRIP_HIGH              0x0008
#define FILTER_FLAG_ENCODE_LOW              0x0010
#define FILTER_FLAG_ENCODE_HIGH             0x0020
#define FILTER_FLAG_ENCODE_AMP              0x0040
#define FILTER_FLAG_NO_ENCODE_QUOTES        0x0080
#define FILTER_FLAG_EMPTY_STRING_NULL       0x0100

#define FILTER_FLAG_ALLOW_FRACTION          0x1000
#define FILTER_FLAG_ALLOW_THOUSAND          0x2000
#define FILTER_FLAG_ALLOW_SCIENTIFIC        0x4000

#define FL_INT           0x0101
#define FL_BOOLEAN       0x0102
#define FL_FLOAT         0x0103

#define FL_REGEXP        0x0110
#define FL_URL           0x0111
#define FL_EMAIL         0x0112
#define FL_IP            0x0113

#define FL_ALL           0x0100

#define FS_DEFAULT       0x0201

#define FS_STRING        0x0201
#define FS_ENCODED       0x0202
#define FS_SPECIAL_CHARS 0x0203
#define FS_UNSAFE_RAW    0x0204
#define FS_EMAIL         0x0205
#define FS_URL           0x0206
#define FS_NUMBER_INT    0x0207
#define FS_NUMBER_FLOAT  0x0208
#define FS_MAGIC_QUOTES  0x0209

#define FS_ALL           0x0200

#define FC_CALLBACK      0x0400

