#!ruby -s
$output ||= "callback"
$out = open("#{$output}.c", "w")

$dl_h = ARGV[0] || "dl.h"

# import DLSTACK_SIZE, DLSTACK_ARGS and so on
File.open($dl_h){|f|
  pre = ""
  f.each{|line|
    line.chop!
    if( line[-1] == ?\\ )
      line.chop!
      line.concat(" ")
      pre += line
      next
    end
    if( pre.size > 0 )
      line = pre + line
      pre  = ""
    end
    case line
    when /#define\s+DLSTACK_SIZE\s+\(?(\d+)\)?/
      DLSTACK_SIZE = $1.to_i
    when /#define\s+DLSTACK_ARGS\s+(.+)/
      DLSTACK_ARGS = $1.to_i
    when /#define\s+DLTYPE_([A-Z_]+)\s+\(?(\d+)\)?/
      eval("#{$1} = #{$2}")
    when /#define\s+MAX_DLTYPE\s+\(?(\d+)\)?/
      MAX_DLTYPE  = $1.to_i
    when /#define\s+MAX_CALLBACK\s+\(?(\d+)\)?/
      MAX_CALLBACK = $1.to_i
    end
  }
}

CDECL = "cdecl"
STDCALL = "stdcall"

CALLTYPES = [CDECL, STDCALL]

DLTYPE = {
  VOID => {
    :name => 'void',
    :type => 'void',
    :conv => nil,
  },
  CHAR => {
    :name => 'char',
    :type => 'char',
    :conv => 'NUM2CHR(%s)'
  },
  SHORT => {
    :name => 'short',
    :type => 'short',
    :conv => 'NUM2INT(%s)',
  },
  INT => {
    :name => 'int',
    :type => 'int',
    :conv => 'NUM2INT(%s)',
  },
  LONG  => {
    :name => 'long',
    :type => 'long',
    :conv => 'NUM2LONG(%s)',
  },
  LONG_LONG => {
    :name => 'long_long',
    :type => 'LONG_LONG',
    :conv => 'NUM2LL(%s)',
  },
  FLOAT => {
    :name => 'float',
    :type => 'float',
    :conv => '(float)RFLOAT_VALUE(%s)',
  },
  DOUBLE => {
    :name => 'double',
    :type => 'double',
    :conv => 'RFLOAT_VALUE(%s)',
  },
  VOIDP => {
    :name => 'ptr',
    :type => 'void *',
    :conv => 'NUM2PTR(%s)',
  },
}


def func_name(ty, argc, n, calltype)
  "rb_dl_callback_#{DLTYPE[ty][:name]}_#{argc}_#{n}_#{calltype}"
end

$out << (<<EOS)
#include "ruby.h"

VALUE rb_DLCdeclCallbackAddrs, rb_DLCdeclCallbackProcs;
#ifdef FUNC_STDCALL
VALUE rb_DLStdcallCallbackAddrs, rb_DLStdcallCallbackProcs;
#endif
/*static void *cdecl_callbacks[MAX_DLTYPE][MAX_CALLBACK];*/
#ifdef FUNC_STDCALL
/*static void *stdcall_callbacks[MAX_DLTYPE][MAX_CALLBACK];*/
#endif
ID   rb_dl_cb_call;
EOS

def foreach_proc_entry
  for calltype in CALLTYPES
    case calltype
    when CDECL
      proc_entry = "rb_DLCdeclCallbackProcs"
    when STDCALL
      proc_entry = "rb_DLStdcallCallbackProcs"
    else
      raise "unknown calltype: #{calltype}"
    end
    yield calltype, proc_entry
  end
end

def gencallback(ty, calltype, proc_entry, argc, n)
  dltype = DLTYPE[ty]
  ret = dltype[:conv]
  src = <<-EOS
#{calltype == STDCALL ? "\n#ifdef FUNC_STDCALL" : ""}
static #{dltype[:type]}
FUNC_#{calltype.upcase}(#{func_name(ty,argc,n,calltype)})(#{(0...argc).collect{|i| "DLSTACK_TYPE stack#{i}"}.join(", ")})
{
    VALUE #{ret ? "ret, " : ""}cb#{argc > 0 ? ", args[#{argc}]" : ""};
#{
      (0...argc).collect{|i|
        "\n    args[#{i}] = PTR2NUM(stack#{i});"
      }.join("")
}
    cb = rb_ary_entry(rb_ary_entry(#{proc_entry}, #{ty}), #{(n * DLSTACK_SIZE) + argc});
    #{ret ? "ret = " : ""}rb_funcall2(cb, rb_dl_cb_call, #{argc}, #{argc > 0 ? 'args' : 'NULL'});
  EOS
  src << "    return #{ret % "ret"};\n" if ret
  src << <<-EOS
}
#{calltype == STDCALL ? "#endif\n" : ""}
  EOS
end

def gen_push_proc_ary(ty, aryname)
  sprintf("    rb_ary_push(#{aryname}, rb_ary_new3(%d,%s));",
          MAX_CALLBACK * DLSTACK_SIZE,
          (0...MAX_CALLBACK).collect{
            (0...DLSTACK_SIZE).collect{ "Qnil" }.join(",")
          }.join(","))
end

def gen_push_addr_ary(ty, aryname, calltype)
  sprintf("    rb_ary_push(#{aryname}, rb_ary_new3(%d,%s));",
          MAX_CALLBACK * DLSTACK_SIZE,
          (0...MAX_CALLBACK).collect{|i|
            (0...DLSTACK_SIZE).collect{|argc|
              "PTR2NUM(%s)" % func_name(ty,argc,i,calltype)
            }.join(",")
          }.join(","))
end

def gen_callback_file(ty)
  filename = "#{$output}-#{ty}.c"
  initname = "rb_dl_init_callbacks_#{ty}"
  body = <<-EOS
#include "dl.h"

extern VALUE rb_DLCdeclCallbackAddrs, rb_DLCdeclCallbackProcs;
#ifdef FUNC_STDCALL
extern VALUE rb_DLStdcallCallbackAddrs, rb_DLStdcallCallbackProcs;
#endif
extern ID   rb_dl_cb_call;
    EOS
  yield body
  body << <<-EOS
void
#{initname}()
{
#{gen_push_proc_ary(ty, "rb_DLCdeclCallbackProcs")}
#{gen_push_addr_ary(ty, "rb_DLCdeclCallbackAddrs", CDECL)}
#ifdef FUNC_STDCALL
#{gen_push_proc_ary(ty, "rb_DLStdcallCallbackProcs")}
#{gen_push_addr_ary(ty, "rb_DLStdcallCallbackAddrs", STDCALL)}
#endif
}
    EOS
  [filename, initname, body]
end

callbacks = []
for ty in 0...MAX_DLTYPE
  filename, initname, body = gen_callback_file(ty) {|f|
    foreach_proc_entry do |calltype, proc_entry|
      for argc in 0...DLSTACK_SIZE
        for n in 0...MAX_CALLBACK
          f << gencallback(ty, calltype, proc_entry, argc, n)
        end
      end
    end
  }
  $out << "void #{initname}();\n"
  callbacks << [filename, body]
end

$out << (<<EOS)
void
Init_callback(void)
{
    VALUE tmp;
    VALUE rb_mDL = rb_path2class("DL");

    rb_dl_cb_call = rb_intern("call");

    tmp = rb_DLCdeclCallbackProcs = rb_ary_new();
    rb_define_const(rb_mDL, "CdeclCallbackProcs", tmp);

    tmp = rb_DLCdeclCallbackAddrs = rb_ary_new();
    rb_define_const(rb_mDL, "CdeclCallbackAddrs", tmp);

#ifdef FUNC_STDCALL
    tmp = rb_DLStdcallCallbackProcs = rb_ary_new();
    rb_define_const(rb_mDL, "StdcallCallbackProcs", tmp);

    tmp = rb_DLStdcallCallbackAddrs = rb_ary_new();
    rb_define_const(rb_mDL, "StdcallCallbackAddrs", tmp);
#endif

#{
    (0...MAX_DLTYPE).collect{|ty|
      "    rb_dl_init_callbacks_#{ty}();"
    }.join("\n")
}
}
EOS
$out.close

for filename, body in callbacks
  open(filename, "wb") {|f| f.puts body}
end
