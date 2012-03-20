#if   defined __GNUC__
#warning use "ruby/st.h" instead of bare "st.h"
#elif defined _MSC_VER || defined __BORLANDC__
#pragma message("warning: use \"ruby/st.h\" instead of bare \"st.h\"")
#endif
#include "ruby/st.h"
