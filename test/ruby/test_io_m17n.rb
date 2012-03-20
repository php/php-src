require 'test/unit'
require 'tmpdir'
require 'timeout'
require_relative 'envutil'

class TestIO_M17N < Test::Unit::TestCase
  ENCS = [
    Encoding::ASCII_8BIT,
    Encoding::EUC_JP,
    Encoding::Shift_JIS,
    Encoding::UTF_8
  ]

  def with_tmpdir
    Dir.mktmpdir {|dir|
      Dir.chdir(dir) {
        yield dir
      }
    }
  end

  def pipe(*args, wp, rp)
    re, we = nil, nil
    r, w = IO.pipe(*args)
    rt = Thread.new do
      begin
        rp.call(r)
      rescue Exception
        r.close
        re = $!
      end
    end
    wt = Thread.new do
      begin
        wp.call(w)
      rescue Exception
        w.close
        we = $!
      end
    end
    flunk("timeout") unless wt.join(10) && rt.join(10)
  ensure
    w.close unless !w || w.closed?
    r.close unless !r || r.closed?
    (wt.kill; wt.join) if wt
    (rt.kill; rt.join) if rt
    raise we if we
    raise re if re
  end

  def with_pipe(*args)
    r, w = IO.pipe(*args)
    begin
      yield r, w
    ensure
      r.close if !r.closed?
      w.close if !w.closed?
    end
  end

  def generate_file(path, content)
    open(path, "wb") {|f| f.write content }
  end

  def encdump(str)
    "#{str.dump}.force_encoding(#{str.encoding.name.dump})"
  end

  def assert_str_equal(expected, actual, message=nil)
    full_message = build_message(message, <<EOT)
#{encdump expected} expected but not equal to
#{encdump actual}.
EOT
    assert_block(full_message) { expected == actual }
  end

  def test_open_r
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r") {|f|
        assert_equal(Encoding.default_external, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_rb
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "rb") {|f|
        assert_equal(Encoding.find("ASCII-8BIT"), f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_enc
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r:euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_enc_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", encoding: "euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_encname_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", encoding: Encoding::EUC_JP) {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_ext_enc_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", external_encoding: Encoding::EUC_JP) {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_ext_encname_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", external_encoding: "euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_r_enc_enc
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", external_encoding: Encoding::EUC_JP, internal_encoding: Encoding::UTF_8) {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_r_encname_encname
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r:euc-jp:utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_r_encname_encname_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", encoding: "euc-jp:utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_r_enc_enc_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", external_encoding: Encoding::EUC_JP, internal_encoding: Encoding::UTF_8) {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_r_externalencname_internalencname_in_opt
    with_tmpdir {
      generate_file('tmp', "")
      open("tmp", "r", external_encoding: "euc-jp", internal_encoding: "utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_w
    with_tmpdir {
      open("tmp", "w") {|f|
        assert_equal(nil, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_wb
    with_tmpdir {
      open("tmp", "wb") {|f|
        assert_equal(Encoding.find("ASCII-8BIT"), f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc
    with_tmpdir {
      open("tmp", "w:euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_in_opt
    with_tmpdir {
      open("tmp", "w", encoding: "euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_in_opt2
    with_tmpdir {
      open("tmp", "w", external_encoding: "euc-jp") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_enc
    with_tmpdir {
      open("tmp", "w:euc-jp:utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_enc_in_opt
    with_tmpdir {
      open("tmp", "w", encoding: "euc-jp:utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_enc_in_opt2
    with_tmpdir {
      open("tmp", "w", external_encoding: "euc-jp", internal_encoding: "utf-8") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_open_w_enc_enc_perm
    with_tmpdir {
      open("tmp", "w:euc-jp:utf-8", 0600) {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
      }
    }
  end

  def test_io_new_enc
    with_tmpdir {
      generate_file("tmp", "\xa1")
      fd = IO.sysopen("tmp")
      f = IO.new(fd, "r:sjis")
      begin
        assert_equal(Encoding::Windows_31J, f.read.encoding)
      ensure
        f.close
      end
    }
  end

  def test_s_pipe_invalid
    pipe("utf-8", "euc-jp", { :invalid=>:replace },
        proc do |w|
          w << "\x80"
          w.close
        end,
        proc do |r|
          assert_equal("?", r.read)
        end)
  end

  def test_s_pipe_undef
    pipe("utf-8:euc-jp", { :undef=>:replace },
         proc do |w|
           w << "\ufffd"
           w.close
         end,
         proc do |r|
           assert_equal("?", r.read)
         end)
  end

  def test_s_pipe_undef_replace_string
    pipe("utf-8:euc-jp", { :undef=>:replace, :replace=>"X" },
         proc do |w|
           w << "\ufffd"
           w.close
         end,
         proc do |r|
           assert_equal("X", r.read)
         end)
  end

  def test_dup
    pipe("utf-8:euc-jp",
         proc do |w|
           w << "\u3042"
           w.close
         end,
         proc do |r|
           r2 = r.dup
           begin
             assert_equal("\xA4\xA2".force_encoding("euc-jp"), r2.read)
           ensure
             r2.close
           end
         end)
  end

  def test_dup_undef
    pipe("utf-8:euc-jp", { :undef=>:replace },
         proc do |w|
           w << "\uFFFD"
           w.close
         end,
         proc do |r|
           r2 = r.dup
           begin
             assert_equal("?", r2.read)
           ensure
             r2.close
           end
         end)
  end

  def test_stdin
    assert_equal(Encoding.default_external, STDIN.external_encoding)
    assert_equal(nil, STDIN.internal_encoding)
  end

  def test_stdout
    assert_equal(nil, STDOUT.external_encoding)
    assert_equal(nil, STDOUT.internal_encoding)
  end

  def test_stderr
    assert_equal(nil, STDERR.external_encoding)
    assert_equal(nil, STDERR.internal_encoding)
  end

  def test_terminator_conversion
    with_tmpdir {
      generate_file('tmp', "before \u00FF after")
      s = open("tmp", "r:utf-8:iso-8859-1") {|f|
        f.gets("\xFF".force_encoding("iso-8859-1"))
      }
      assert_equal(Encoding.find("iso-8859-1"), s.encoding)
      assert_str_equal("before \xFF".force_encoding("iso-8859-1"), s, '[ruby-core:14288]')
    }
  end

  def test_terminator_conversion2
    with_tmpdir {
      generate_file('tmp', "before \xA1\xA2\xA2\xA3 after")
      s = open("tmp", "r:euc-jp:utf-8") {|f|
        f.gets("\xA2\xA2".force_encoding("euc-jp").encode("utf-8"))
      }
      assert_equal(Encoding.find("utf-8"), s.encoding)
      assert_str_equal("before \xA1\xA2\xA2\xA3 after".force_encoding("euc-jp").encode("utf-8"), s, '[ruby-core:14319]')
    }
  end

  def test_terminator_stateful_conversion
    with_tmpdir {
      src = "before \e$B\x23\x30\x23\x31\e(B after".force_encoding("iso-2022-jp")
      generate_file('tmp', src)
      s = open("tmp", "r:iso-2022-jp:euc-jp") {|f|
        f.gets("0".force_encoding("euc-jp"))
      }
      assert_equal(Encoding.find("euc-jp"), s.encoding)
      assert_str_equal(src.encode("euc-jp"), s)
    }
  end

  def test_nonascii_terminator
    with_tmpdir {
      generate_file('tmp', "before \xA2\xA2 after")
      open("tmp", "r:euc-jp") {|f|
        assert_raise(ArgumentError) {
          f.gets("\xA2\xA2".force_encoding("utf-8"))
        }
      }
    }
  end

  def test_pipe_terminator_conversion
    rs = "\xA2\xA2".encode("utf-8", "euc-jp")
    pipe("euc-jp:utf-8",
         proc do |w|
           w.write "before \xa2\xa2 after"
           w.close
         end,
         proc do |r|
           timeout(1) {
             assert_equal("before \xa2\xa2".encode("utf-8", "euc-jp"),
                          r.gets(rs))
           }
         end)
  end

  def test_pipe_conversion
    pipe("euc-jp:utf-8",
         proc do |w|
           w.write "\xa1\xa1"
         end,
         proc do |r|
           assert_equal("\xa1\xa1".encode("utf-8", "euc-jp"), r.getc)
         end)
  end

  def test_pipe_convert_partial_read
    pipe("euc-jp:utf-8",
         proc do |w|
           w.write "\xa1"
           sleep 0.1
           w.write "\xa1"
         end,
         proc do |r|
           assert_equal("\xa1\xa1".encode("utf-8", "euc-jp"), r.getc)
         end)
  end

  def test_getc_invalid
    pipe("euc-jp:utf-8",
         proc do |w|
           w << "\xa1xyz"
           w.close
         end,
         proc do |r|
           err = assert_raise(Encoding::InvalidByteSequenceError) { r.getc }
           assert_equal("\xA1".force_encoding("ascii-8bit"), err.error_bytes)
           assert_equal("xyz", r.read(10))
         end)
  end

  def test_getc_stateful_conversion
    with_tmpdir {
      src = "\e$B\x23\x30\x23\x31\e(B".force_encoding("iso-2022-jp")
      generate_file('tmp', src)
      open("tmp", "r:iso-2022-jp:euc-jp") {|f|
        assert_equal("\xa3\xb0".force_encoding("euc-jp"), f.getc)
        assert_equal("\xa3\xb1".force_encoding("euc-jp"), f.getc)
      }
    }
  end

  def test_getc_newlineconv
    with_tmpdir {
      src = "\u3042"
      generate_file('tmp', src)
      defext = Encoding.default_external
      Encoding.default_external = Encoding::UTF_8
      open("tmp", "rt") {|f|
        s = f.getc
        assert_equal(true, s.valid_encoding?)
        assert_equal("\u3042", s)
      }
      Encoding.default_external = defext
    }
  end

  def test_getc_newlineconv_invalid
    with_tmpdir {
      src = "\xE3\x81"
      generate_file('tmp', src)
      defext = Encoding.default_external
      Encoding.default_external = Encoding::UTF_8
      open("tmp", "rt") {|f|
        s = f.getc
        assert_equal(false, s.valid_encoding?)
        assert_equal("\xE3".force_encoding("UTF-8"), s)
        s = f.getc
        assert_equal(false, s.valid_encoding?)
        assert_equal("\x81".force_encoding("UTF-8"), s)
      }
      Encoding.default_external = defext
    }
  end

  def test_ungetc_int
    with_tmpdir {
      generate_file('tmp', "A")
      s = open("tmp", "r:GB18030") {|f|
        f.ungetc(0x8431A439)
        f.read
      }
      assert_equal(Encoding::GB18030, s.encoding)
      assert_str_equal(0x8431A439.chr("GB18030")+"A", s)
    }
  end

  def test_ungetc_str
    with_tmpdir {
      generate_file('tmp', "A")
      s = open("tmp", "r:GB18030") {|f|
        f.ungetc(0x8431A439.chr("GB18030"))
        f.read
      }
      assert_equal(Encoding::GB18030, s.encoding)
      assert_str_equal(0x8431A439.chr("GB18030")+"A", s)
    }
  end

  def test_ungetc_stateful_conversion
    with_tmpdir {
      src = "before \e$B\x23\x30\x23\x31\e(B after".force_encoding("iso-2022-jp")
      generate_file('tmp', src)
      s = open("tmp", "r:iso-2022-jp:euc-jp") {|f|
        f.ungetc("0".force_encoding("euc-jp"))
        f.read
      }
      assert_equal(Encoding.find("euc-jp"), s.encoding)
      assert_str_equal("0" + src.encode("euc-jp"), s)
    }
  end

  def test_ungetc_stateful_conversion2
    with_tmpdir {
      src =    "before \e$B\x23\x30\x23\x31\e(B after".force_encoding("iso-2022-jp")
      former = "before \e$B\x23\x30\e(B".force_encoding("iso-2022-jp")
      rs =            "\e$B\x23\x30\e(B".force_encoding("iso-2022-jp")
      latter =                "\e$B\x23\x31\e(B after".force_encoding("iso-2022-jp")
      generate_file('tmp', src)
      s = open("tmp", "r:iso-2022-jp:euc-jp") {|f|
        assert_equal(former.encode("euc-jp", "iso-2022-jp"),
                     f.gets(rs.encode("euc-jp", "iso-2022-jp")))
        f.ungetc("0")
        f.read
      }
      assert_equal(Encoding.find("euc-jp"), s.encoding)
      assert_str_equal("0" + latter.encode("euc-jp"), s)
    }
  end

  def test_open_ascii
    with_tmpdir {
      src = "abc\n"
      generate_file('tmp', "abc\n")
      ENCS.each {|enc|
        s = open('tmp', "r:#{enc}") {|f| f.gets }
        assert_equal(enc, s.encoding)
        assert_str_equal(src, s)
      }
    }
  end

  def test_open_nonascii
    with_tmpdir {
      src = "\xc2\xa1\n"
      generate_file('tmp', src)
      ENCS.each {|enc|
        content = src.dup.force_encoding(enc)
        s = open('tmp', "r:#{enc}") {|f| f.gets }
        assert_equal(enc, s.encoding)
        assert_str_equal(content, s)
      }
    }
  end

  def test_read_encoding
    with_tmpdir {
      src = "\xc2\xa1\n".force_encoding("ASCII-8BIT")
      generate_file('tmp', "\xc2\xa1\n")
      ENCS.each {|enc|
        content = src.dup.force_encoding(enc)
        open('tmp', "r:#{enc}") {|f|
          s = f.getc
          assert_equal(enc, s.encoding)
          assert_str_equal(content[0], s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.readchar
          assert_equal(enc, s.encoding)
          assert_str_equal(content[0], s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.gets
          assert_equal(enc, s.encoding)
          assert_str_equal(content, s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.readline
          assert_equal(enc, s.encoding)
          assert_str_equal(content, s)
        }
        open('tmp', "r:#{enc}") {|f|
          lines = f.readlines
          assert_equal(1, lines.length)
          s = lines[0]
          assert_equal(enc, s.encoding)
          assert_str_equal(content, s)
        }
        open('tmp', "r:#{enc}") {|f|
          f.each_line {|s|
            assert_equal(enc, s.encoding)
            assert_str_equal(content, s)
          }
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.read
          assert_equal(enc, s.encoding)
          assert_str_equal(content, s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.read(1)
          assert_equal(Encoding::ASCII_8BIT, s.encoding)
          assert_str_equal(src[0], s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.readpartial(1)
          assert_equal(Encoding::ASCII_8BIT, s.encoding)
          assert_str_equal(src[0], s)
        }
        open('tmp', "r:#{enc}") {|f|
          s = f.sysread(1)
          assert_equal(Encoding::ASCII_8BIT, s.encoding)
          assert_str_equal(src[0], s)
        }
      }
    }
  end

  def test_write_noenc
    src = "\xc2\xa1\n".force_encoding("ascii-8bit")
    with_tmpdir {
      open('tmp', "w") {|f|
        ENCS.each {|enc|
          f.write src.dup.force_encoding(enc)
        }
      }
      open('tmp', 'r:ascii-8bit') {|f|
        assert_equal(src*ENCS.length, f.read)
      }
    }
  end

  def test_write_conversion
    utf8 = "\u6666"
    eucjp = "\xb3\xa2".force_encoding("EUC-JP")
    with_tmpdir {
      open('tmp', "w:EUC-JP") {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(nil, f.internal_encoding)
        f.print utf8
      }
      assert_equal(eucjp, File.read('tmp').force_encoding("EUC-JP"))
      open('tmp', 'r:EUC-JP:UTF-8') {|f|
        assert_equal(Encoding::EUC_JP, f.external_encoding)
        assert_equal(Encoding::UTF_8, f.internal_encoding)
        assert_equal(utf8, f.read)
      }
    }
  end

  def test_pipe
    utf8 = "\u6666"
    eucjp = "\xb3\xa2".force_encoding("EUC-JP")

    pipe(proc do |w|
      w << utf8
      w.close
    end, proc do |r|
      assert_equal(Encoding.default_external, r.external_encoding)
      assert_equal(nil, r.internal_encoding)
      s = r.read
      assert_equal(Encoding.default_external, s.encoding)
      assert_str_equal(utf8.dup.force_encoding(Encoding.default_external), s)
    end)

    pipe("EUC-JP",
         proc do |w|
           w << eucjp
           w.close
         end,
         proc do |r|
           assert_equal(Encoding::EUC_JP, r.external_encoding)
           assert_equal(nil, r.internal_encoding)
           assert_equal(eucjp, r.read)
         end)

    pipe("UTF-8",
         proc do |w|
           w << "a" * 1023 + "\u3042" + "a" * 1022
           w.close
         end,
         proc do |r|
           assert_equal(true, r.read.valid_encoding?)
         end)

    pipe("UTF-8:EUC-JP",
         proc do |w|
           w << utf8
           w.close
         end,
         proc do |r|
           assert_equal(Encoding::UTF_8, r.external_encoding)
           assert_equal(Encoding::EUC_JP, r.internal_encoding)
           assert_equal(eucjp, r.read)
         end)

    e = assert_raise(ArgumentError) {with_pipe("UTF-8", "UTF-8".encode("UTF-32BE")) {}}
    assert_match(/invalid name encoding/, e.message)
    e = assert_raise(ArgumentError) {with_pipe("UTF-8".encode("UTF-32BE")) {}}
    assert_match(/invalid name encoding/, e.message)

    ENCS.each {|enc|
      pipe(enc,
           proc do |w|
             w << "\xc2\xa1"
             w.close
           end,
           proc do |r|
             s = r.getc
             assert_equal(enc, s.encoding)
           end)
    }

    ENCS.each {|enc|
      next if enc == Encoding::ASCII_8BIT
      next if enc == Encoding::UTF_8
      pipe("#{enc}:UTF-8",
           proc do |w|
             w << "\xc2\xa1"
             w.close
           end,
           proc do |r|
             s = r.read
             assert_equal(Encoding::UTF_8, s.encoding)
             assert_equal(s.encode("UTF-8"), s)
           end)
    }

  end

  def test_marshal
    data = 56225
    pipe("EUC-JP",
         proc do |w|
           Marshal.dump(data, w)
           w.close
         end,
         proc do |r|
           result = nil
           assert_nothing_raised("[ruby-dev:33264]") { result = Marshal.load(r) }
           assert_equal(data, result)
         end)
  end

  def test_gets_nil
    pipe("UTF-8:EUC-JP",
         proc do |w|
           w << "\u{3042}"
           w.close
         end,
         proc do |r|
           result = r.gets(nil)
           assert_equal("\u{3042}".encode("euc-jp"), result)
         end)
  end

  def test_gets_limit
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.gets(1)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.gets(2)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4".force_encoding("euc-jp"), r.gets(3)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4".force_encoding("euc-jp"), r.gets(4)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4\xa4\xa6".force_encoding("euc-jp"), r.gets(5)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4\xa4\xa6".force_encoding("euc-jp"), r.gets(6)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4\xa4\xa6\n".force_encoding("euc-jp"), r.gets(7)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4\xa4\xa6\n".force_encoding("euc-jp"), r.gets(8)) })
    pipe("euc-jp",
         proc {|w| w << "\xa4\xa2\xa4\xa4\xa4\xa6\n\xa4\xa8\xa4\xaa"; w.close },
         proc {|r| assert_equal("\xa4\xa2\xa4\xa4\xa4\xa6\n".force_encoding("euc-jp"), r.gets(9)) })
  end

  def test_gets_invalid
    before = "\u{3042}\u{3044}"
    invalid = "\x80".force_encoding("utf-8")
    after = "\u{3046}\u{3048}"
    pipe("utf-8:euc-jp",
         proc do |w|
           w << before + invalid + after
           w.close
         end,
         proc do |r|
           err = assert_raise(Encoding::InvalidByteSequenceError) { r.gets }
           assert_equal(invalid.force_encoding("ascii-8bit"), err.error_bytes)
           assert_equal(after.encode("euc-jp"), r.gets)
         end)
  end

  def test_getc_invalid2
    before1 = "\u{3042}"
    before2 = "\u{3044}"
    invalid = "\x80".force_encoding("utf-8")
    after1 = "\u{3046}"
    after2 = "\u{3048}"
    pipe("utf-8:euc-jp",
         proc do |w|
           w << before1 + before2 + invalid + after1 + after2
           w.close
         end,
         proc do |r|
           assert_equal(before1.encode("euc-jp"), r.getc)
           assert_equal(before2.encode("euc-jp"), r.getc)
           err = assert_raise(Encoding::InvalidByteSequenceError) { r.getc }
           assert_equal(invalid.force_encoding("ascii-8bit"), err.error_bytes)
           assert_equal(after1.encode("euc-jp"), r.getc)
           assert_equal(after2.encode("euc-jp"), r.getc)
         end)
  end

  def test_getc_invalid3
    before1 = "\x42\x30".force_encoding("utf-16le")
    before2 = "\x44\x30".force_encoding("utf-16le")
    invalid = "\x00\xd8".force_encoding("utf-16le")
    after1 = "\x46\x30".force_encoding("utf-16le")
    after2 = "\x48\x30".force_encoding("utf-16le")
    pipe("utf-16le:euc-jp", { :binmode => true },
         proc do |w|
           w << before1 + before2 + invalid + after1 + after2
           w.close
         end,
         proc do |r|
           assert_equal(before1.encode("euc-jp"), r.getc)
           assert_equal(before2.encode("euc-jp"), r.getc)
           err = assert_raise(Encoding::InvalidByteSequenceError) { r.getc }
           assert_equal(invalid.force_encoding("ascii-8bit"), err.error_bytes)
           assert_equal(after1.encode("euc-jp"), r.getc)
           assert_equal(after2.encode("euc-jp"), r.getc)
         end)
  end

  def test_read_all
    str = "\u3042\u3044"
    pipe("utf-8:euc-jp",
         proc do |w|
           w << str
           w.close
         end,
         proc do |r|
           assert_equal(str.encode("euc-jp"), r.read)
         end)
  end

  def test_read_all_invalid
    before = "\u{3042}\u{3044}"
    invalid = "\x80".force_encoding("utf-8")
    after = "\u{3046}\u{3048}"
    pipe("utf-8:euc-jp",
      proc do |w|
        w << before + invalid + after
        w.close
      end,
      proc do |r|
        err = assert_raise(Encoding::InvalidByteSequenceError) { r.read }
        assert_equal(invalid.force_encoding("ascii-8bit"), err.error_bytes)
        assert_equal(after.encode("euc-jp"), r.read)
      end)
  end

  def test_file_foreach
    with_tmpdir {
      generate_file('tst', 'a' * 8191 + "\xa1\xa1")
      assert_nothing_raised {
        File.foreach('tst', :encoding=>"euc-jp") {|line| line.inspect }
      }
    }
  end

  def test_set_encoding
    pipe("utf-8:euc-jp",
         proc do |w|
           s = "\u3042".force_encoding("ascii-8bit")
           s << "\x82\xa0".force_encoding("ascii-8bit")
           w << s
           w.close
         end,
         proc do |r|
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
           r.set_encoding("shift_jis:euc-jp")
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
         end)
  end

  def test_set_encoding2
    pipe("utf-8:euc-jp",
         proc do |w|
           s = "\u3042".force_encoding("ascii-8bit")
           s << "\x82\xa0".force_encoding("ascii-8bit")
           w << s
           w.close
         end,
         proc do |r|
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
           r.set_encoding("shift_jis", "euc-jp")
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
         end)
  end

  def test_set_encoding_nil
    pipe("utf-8:euc-jp",
         proc do |w|
           s = "\u3042".force_encoding("ascii-8bit")
           s << "\x82\xa0".force_encoding("ascii-8bit")
           w << s
           w.close
         end,
         proc do |r|
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
           r.set_encoding(nil)
           assert_equal("\x82\xa0".force_encoding(Encoding.default_external), r.read)
         end)
  end

  def test_set_encoding_enc
    pipe("utf-8:euc-jp",
         proc do |w|
           s = "\u3042".force_encoding("ascii-8bit")
           s << "\x82\xa0".force_encoding("ascii-8bit")
           w << s
           w.close
         end,
         proc do |r|
           assert_equal("\xa4\xa2".force_encoding("euc-jp"), r.getc)
           r.set_encoding(Encoding::Shift_JIS)
           assert_equal("\x82\xa0".force_encoding(Encoding::Shift_JIS), r.getc)
         end)
  end

  def test_set_encoding_invalid
    pipe(proc do |w|
           w << "\x80"
           w.close
         end,
         proc do |r|
           r.set_encoding("utf-8:euc-jp", :invalid=>:replace)
           assert_equal("?", r.read)
         end)
  end

  def test_set_encoding_identical
    bug5568 = '[ruby-core:40727]'
    open(__FILE__, "r") do |f|
      assert_warn(/Ignoring internal encoding euc-jp: it is identical to external encoding eucjp/, bug5568) {
        f.set_encoding("eucjp:euc-jp")
      }
      assert_warn(/Ignoring internal encoding euc-jp: it is identical to external encoding eucjp/, bug5568) {
        f.set_encoding("eucjp", "euc-jp")
      }
      assert_warn(/Ignoring internal encoding euc-jp: it is identical to external encoding EUC-JP/, bug5568) {
        f.set_encoding(Encoding::EUC_JP, "euc-jp")
      }
      assert_warn(/Ignoring internal encoding EUC-JP: it is identical to external encoding eucjp/, bug5568) {
        f.set_encoding("eucjp", Encoding::EUC_JP)
      }
      assert_warn(/Ignoring internal encoding EUC-JP: it is identical to external encoding EUC-JP/, bug5568) {
        f.set_encoding(Encoding::EUC_JP, Encoding::EUC_JP)
      }
      nonstr = Object.new
      def nonstr.to_str; "eucjp"; end
      assert_warn(/Ignoring internal encoding eucjp: it is identical to external encoding eucjp/, bug5568) {
        f.set_encoding(nonstr, nonstr)
      }
    end
  end

  def test_set_encoding_undef
    pipe(proc do |w|
           w << "\ufffd"
           w.close
         end,
         proc do |r|
           r.set_encoding("utf-8", "euc-jp", :undef=>:replace)
           assert_equal("?", r.read)
         end)
  end

  def test_set_encoding_undef_replace
    pipe(proc do |w|
           w << "\ufffd"
           w.close
         end,
         proc do |r|
           r.set_encoding("utf-8", "euc-jp", :undef=>:replace, :replace=>"ZZZ")
           assert_equal("ZZZ", r.read)
         end)
    pipe(proc do |w|
           w << "\ufffd"
           w.close
         end,
         proc do |r|
           r.set_encoding("utf-8:euc-jp", :undef=>:replace, :replace=>"ZZZ")
           assert_equal("ZZZ", r.read)
         end)
  end

  def test_set_encoding_binmode
    assert_raise(ArgumentError) {
      open(__FILE__, "rt") {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_raise(ArgumentError) {
      open(__FILE__, "r") {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_nothing_raised {
      open(__FILE__, "rb") {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_nothing_raised {
      open(__FILE__, "r") {|f|
        f.binmode
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_nothing_raised {
      open(__FILE__, "rt") {|f|
        f.binmode
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_nothing_raised {
      open(__FILE__, "r", binmode: true) {|f|
        assert_equal(Encoding::ASCII_8BIT, f.external_encoding)
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_raise(ArgumentError) {
      open(__FILE__, "rb", binmode: true) {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_raise(ArgumentError) {
      open(__FILE__, "rb", binmode: false) {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
  end

  def test_set_encoding_unsupported
    bug5567 = '[ruby-core:40726]'
    IO.pipe do |r, w|
      assert_nothing_raised(bug5567) do
        assert_warning(/Unsupported/, bug5567) {r.set_encoding("fffffffffffxx")}
        assert_warning(/Unsupported/, bug5567) {r.set_encoding("fffffffffffxx", "us-ascii")}
        assert_warning(/Unsupported/, bug5567) {r.set_encoding("us-ascii", "fffffffffffxx")}
      end
    end
  end

  def test_textmode_twice
    assert_raise(ArgumentError) {
      open(__FILE__, "rt", textmode: true) {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
    assert_raise(ArgumentError) {
      open(__FILE__, "rt", textmode: false) {|f|
        f.set_encoding("iso-2022-jp")
      }
    }
  end

  def test_write_conversion_fixenc
    pipe(proc do |w|
           w.set_encoding("iso-2022-jp:utf-8")
             w << "\u3042"
             w << "\u3044"
             w.close
         end,
         proc do |r|
           assert_equal("\e$B$\"$$\e(B".force_encoding("ascii-8bit"),
                        r.read.force_encoding("ascii-8bit"))
         end)
  end

  def test_write_conversion_anyenc_stateful
    pipe(proc do |w|
           w.set_encoding("iso-2022-jp")
           w << "\u3042"
           w << "\x82\xa2".force_encoding("sjis")
           w.close
         end,
         proc do |r|
           assert_equal("\e$B$\"$$\e(B".force_encoding("ascii-8bit"),
                        r.read.force_encoding("ascii-8bit"))
         end)
  end

  def test_write_conversion_anyenc_stateless
    pipe(proc do |w|
           w.set_encoding("euc-jp")
           w << "\u3042"
           w << "\x82\xa2".force_encoding("sjis")
           w.close
         end,
         proc do |r|
           assert_equal("\xa4\xa2\xa4\xa4".force_encoding("ascii-8bit"),
                        r.read.force_encoding("ascii-8bit"))
         end)
  end

  def test_write_conversion_anyenc_stateful_nosync
    pipe(proc do |w|
           w.sync = false
           w.set_encoding("iso-2022-jp")
           w << "\u3042"
           w << "\x82\xa2".force_encoding("sjis")
           w.close
         end,
         proc do |r|
           assert_equal("\e$B$\"$$\e(B".force_encoding("ascii-8bit"),
                        r.read.force_encoding("ascii-8bit"))
         end)
  end

  def test_read_stateful
    pipe("euc-jp:iso-2022-jp",
         proc do |w|
           w << "\xA4\xA2"
           w.close
         end,
         proc do |r|
           assert_equal("\e$B$\"\e(B".force_encoding("iso-2022-jp"), r.read)
         end)
  end

  def test_stdin_external_encoding_with_reopen
    skip "passing non-stdio fds is not supported" if /mswin|mingw/ =~ RUBY_PLATFORM
    with_tmpdir {
      open("tst", "w+") {|f|
        pid = spawn(EnvUtil.rubybin, '-e', <<-'End', 10=>f)
          io = IO.new(10, "r+")
          STDIN.reopen(io)
          STDIN.external_encoding
          STDIN.write "\u3042"
          STDIN.flush
        End
        Process.wait pid
        f.rewind
        result = f.read.force_encoding("ascii-8bit")
        assert_equal("\u3042".force_encoding("ascii-8bit"), result)
      }
    }
  end

  def test_popen_r_enc
    IO.popen("#{EnvUtil.rubybin} -e 'putc 255'", "r:ascii-8bit") {|f|
      assert_equal(Encoding::ASCII_8BIT, f.external_encoding)
      assert_equal(nil, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::ASCII_8BIT, s.encoding)
      assert_equal("\xff".force_encoding("ascii-8bit"), s)
    }
  end

  def test_popen_r_enc_in_opt
    IO.popen("#{EnvUtil.rubybin} -e 'putc 255'", "r", encoding: "ascii-8bit") {|f|
      assert_equal(Encoding::ASCII_8BIT, f.external_encoding)
      assert_equal(nil, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::ASCII_8BIT, s.encoding)
      assert_equal("\xff".force_encoding("ascii-8bit"), s)
    }
  end

  def test_popen_r_enc_in_opt2
    IO.popen("#{EnvUtil.rubybin} -e 'putc 255'", "r", external_encoding: "ascii-8bit") {|f|
      assert_equal(Encoding::ASCII_8BIT, f.external_encoding)
      assert_equal(nil, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::ASCII_8BIT, s.encoding)
      assert_equal("\xff".force_encoding("ascii-8bit"), s)
    }
  end

  def test_popen_r_enc_enc
    IO.popen("#{EnvUtil.rubybin} -e 'putc 0xa1'", "r:shift_jis:euc-jp") {|f|
      assert_equal(Encoding::Shift_JIS, f.external_encoding)
      assert_equal(Encoding::EUC_JP, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::EUC_JP, s.encoding)
      assert_equal("\x8e\xa1".force_encoding("euc-jp"), s)
    }
  end

  def test_popen_r_enc_enc_in_opt
    IO.popen("#{EnvUtil.rubybin} -e 'putc 0xa1'", "r", encoding: "shift_jis:euc-jp") {|f|
      assert_equal(Encoding::Shift_JIS, f.external_encoding)
      assert_equal(Encoding::EUC_JP, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::EUC_JP, s.encoding)
      assert_equal("\x8e\xa1".force_encoding("euc-jp"), s)
    }
  end

  def test_popen_r_enc_enc_in_opt2
    IO.popen("#{EnvUtil.rubybin} -e 'putc 0xa1'", "r", external_encoding: "shift_jis", internal_encoding: "euc-jp") {|f|
      assert_equal(Encoding::Shift_JIS, f.external_encoding)
      assert_equal(Encoding::EUC_JP, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::EUC_JP, s.encoding)
      assert_equal("\x8e\xa1".force_encoding("euc-jp"), s)
    }
  end

  def test_popenv_r_enc_enc_in_opt2
    IO.popen([EnvUtil.rubybin, "-e", "putc 0xa1"], "r", external_encoding: "shift_jis", internal_encoding: "euc-jp") {|f|
      assert_equal(Encoding::Shift_JIS, f.external_encoding)
      assert_equal(Encoding::EUC_JP, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::EUC_JP, s.encoding)
      assert_equal("\x8e\xa1".force_encoding("euc-jp"), s)
    }
  end

  def test_open_pipe_r_enc
    open("|#{EnvUtil.rubybin} -e 'putc 255'", "r:ascii-8bit") {|f|
      assert_equal(Encoding::ASCII_8BIT, f.external_encoding)
      assert_equal(nil, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::ASCII_8BIT, s.encoding)
      assert_equal("\xff".force_encoding("ascii-8bit"), s)
    }
  end

  def test_open_pipe_r_enc2
    open("|#{EnvUtil.rubybin} -e 'putc \"\\u3042\"'", "r:UTF-8") {|f|
      assert_equal(Encoding::UTF_8, f.external_encoding)
      assert_equal(nil, f.internal_encoding)
      s = f.read
      assert_equal(Encoding::UTF_8, s.encoding)
      assert_equal("\u3042", s)
    }
  end

  def test_s_foreach_enc
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :mode => "r:ascii-8bit") {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_enc_in_opt
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :encoding => "ascii-8bit") {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_enc_in_opt2
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :external_encoding => "ascii-8bit") {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_enc_enc
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :mode => "r:utf-8:euc-jp") {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_s_foreach_enc_enc_in_opt
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :mode => "r", :encoding => "utf-8:euc-jp") {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_s_foreach_enc_enc_in_opt2
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :mode => "r", :external_encoding => "utf-8", :internal_encoding => "euc-jp") {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :open_args => ["r:ascii-8bit"]) {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc_in_opt
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :open_args => ["r", encoding: "ascii-8bit"]) {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc_in_opt2
    with_tmpdir {
      generate_file("t", "\xff")
      IO.foreach("t", :open_args => ["r", external_encoding: "ascii-8bit"]) {|s|
        assert_equal(Encoding::ASCII_8BIT, s.encoding)
        assert_equal("\xff".force_encoding("ascii-8bit"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc_enc
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :open_args => ["r:utf-8:euc-jp"]) {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc_enc_in_opt
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :open_args => ["r", encoding: "utf-8:euc-jp"]) {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_s_foreach_open_args_enc_enc_in_opt2
    with_tmpdir {
      generate_file("t", "\u3042")
      IO.foreach("t", :open_args => ["r", external_encoding: "utf-8", internal_encoding: "euc-jp"]) {|s|
        assert_equal(Encoding::EUC_JP, s.encoding)
        assert_equal("\xa4\xa2".force_encoding("euc-jp"), s)
      }
    }
  end

  def test_both_textmode_binmode
    assert_raise(ArgumentError) { open("not-exist", "r", :textmode=>true, :binmode=>true) }
    assert_raise(ArgumentError) { open("not-exist", "rt", :binmode=>true) }
    assert_raise(ArgumentError) { open("not-exist", "rb", :textmode=>true) }
  end

  def test_textmode_decode_universal_newline_read
    with_tmpdir {
      generate_file("t.crlf", "a\r\nb\r\nc\r\n")
      assert_equal("a\nb\nc\n", File.read("t.crlf", mode:"rt:euc-jp:utf-8"))
      assert_equal("a\nb\nc\n", File.read("t.crlf", mode:"rt"))
      open("t.crlf", "rt:euc-jp:utf-8") {|f| assert_equal("a\nb\nc\n", f.read) }
      open("t.crlf", "rt") {|f| assert_equal("a\nb\nc\n", f.read) }
      open("t.crlf", "r", :textmode=>true) {|f| assert_equal("a\nb\nc\n", f.read) }
      open("t.crlf", "r", textmode: true, universal_newline: false) {|f|
        assert_equal("a\r\nb\r\nc\r\n", f.read)
      }

      generate_file("t.cr", "a\rb\rc\r")
      assert_equal("a\nb\nc\n", File.read("t.cr", mode:"rt:euc-jp:utf-8"))
      assert_equal("a\nb\nc\n", File.read("t.cr", mode:"rt"))

      generate_file("t.lf", "a\nb\nc\n")
      assert_equal("a\nb\nc\n", File.read("t.cr", mode:"rt:euc-jp:utf-8"))
      assert_equal("a\nb\nc\n", File.read("t.cr", mode:"rt"))
    }
  end

  def test_textmode_decode_universal_newline_getc
    with_tmpdir {
      generate_file("t.crlf", "a\r\nb\r\nc\r\n")
      open("t.crlf", "rt") {|f|
        assert_equal("a", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("b", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("c", f.getc)
        assert_equal("\n", f.getc)
        assert_equal(nil, f.getc)
      }

      generate_file("t.cr", "a\rb\rc\r")
      open("t.cr", "rt") {|f|
        assert_equal("a", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("b", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("c", f.getc)
        assert_equal("\n", f.getc)
        assert_equal(nil, f.getc)
      }

      generate_file("t.lf", "a\nb\nc\n")
      open("t.lf", "rt") {|f|
        assert_equal("a", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("b", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("c", f.getc)
        assert_equal("\n", f.getc)
        assert_equal(nil, f.getc)
      }
    }
  end

  def test_textmode_decode_universal_newline_gets
    with_tmpdir {
      generate_file("t.crlf", "a\r\nb\r\nc\r\n")
      open("t.crlf", "rt") {|f|
        assert_equal("a\n", f.gets)
        assert_equal("b\n", f.gets)
        assert_equal("c\n", f.gets)
        assert_equal(nil, f.gets)
      }

      generate_file("t.cr", "a\rb\rc\r")
      open("t.cr", "rt") {|f|
        assert_equal("a\n", f.gets)
        assert_equal("b\n", f.gets)
        assert_equal("c\n", f.gets)
        assert_equal(nil, f.gets)
      }

      generate_file("t.lf", "a\nb\nc\n")
      open("t.lf", "rt") {|f|
        assert_equal("a\n", f.gets)
        assert_equal("b\n", f.gets)
        assert_equal("c\n", f.gets)
        assert_equal(nil, f.gets)
      }
    }
  end

  def test_textmode_decode_universal_newline_utf16
    with_tmpdir {
      generate_file("t.utf16be.crlf", "\0a\0\r\0\n\0b\0\r\0\n\0c\0\r\0\n")
      assert_equal("a\nb\nc\n", File.read("t.utf16be.crlf", mode:"rt:utf-16be:utf-8"))

      generate_file("t.utf16le.crlf", "a\0\r\0\n\0b\0\r\0\n\0c\0\r\0\n\0")
      assert_equal("a\nb\nc\n", File.read("t.utf16le.crlf", mode:"rt:utf-16le:utf-8"))

      generate_file("t.utf16be.cr", "\0a\0\r\0b\0\r\0c\0\r")
      assert_equal("a\nb\nc\n", File.read("t.utf16be.cr", mode:"rt:utf-16be:utf-8"))

      generate_file("t.utf16le.cr", "a\0\r\0b\0\r\0c\0\r\0")
      assert_equal("a\nb\nc\n", File.read("t.utf16le.cr", mode:"rt:utf-16le:utf-8"))

      generate_file("t.utf16be.lf", "\0a\0\n\0b\0\n\0c\0\n")
      assert_equal("a\nb\nc\n", File.read("t.utf16be.lf", mode:"rt:utf-16be:utf-8"))

      generate_file("t.utf16le.lf", "a\0\n\0b\0\n\0c\0\n\0")
      assert_equal("a\nb\nc\n", File.read("t.utf16le.lf", mode:"rt:utf-16le:utf-8"))
    }
  end

  SYSTEM_NEWLINE = []
  def system_newline
    return SYSTEM_NEWLINE.first if !SYSTEM_NEWLINE.empty?
    with_tmpdir {
      open("newline", "wt") {|f|
        f.print "\n"
      }
      open("newline", "rb") {|f|
        SYSTEM_NEWLINE << f.read
      }
    }
    SYSTEM_NEWLINE.first
  end

  def test_textmode_encode_newline
    with_tmpdir {
      open("t.txt", "wt") {|f|
        f.puts "abc"
        f.puts "def"
      }
      content = File.read("t.txt", :mode=>"rb")
      nl = system_newline
      assert_equal("abc#{nl}def#{nl}", content)
    }
  end

  def test_textmode_encode_newline_enc
    with_tmpdir {
      open("t.txt", "wt:euc-jp") {|f|
        f.puts "abc\u3042"
        f.puts "def\u3044"
      }
      content = File.read("t.txt", :mode=>"rb:ascii-8bit")
      nl = system_newline
      assert_equal("abc\xA4\xA2#{nl}def\xA4\xA4#{nl}", content)
    }
  end

  def test_read_newline_conversion_with_encoding_conversion
    with_tmpdir {
      generate_file("t.utf8.crlf", "a\r\nb\r\n")
      open("t.utf8.crlf", "rb:utf-8:utf-16be") {|f|
        content = f.read
        assert_equal("\0a\0\r\0\n\0b\0\r\0\n".force_encoding("UTF-16BE"), content)
      }
      open("t.utf8.crlf", "rt:utf-8:utf-16be") {|f|
        content = f.read
        assert_equal("\0a\0\n\0b\0\n".force_encoding("UTF-16BE"), content)
      }
      open("t.utf8.crlf", "r:utf-8:utf-16be") {|f|
        content = f.read
        if system_newline == "\n"
          assert_equal("\0a\0\r\0\n\0b\0\r\0\n".force_encoding("UTF-16BE"), content)
        else
          assert_equal("\0a\0\n\0b\0\n".force_encoding("UTF-16BE"), content)
        end
      }
    }
  end

  def test_read_newline_conversion_without_encoding_conversion
    with_tmpdir {
      generate_file("t.utf16.crlf", "\0a\0\r\0\n\0b\0\r\0\n")
      open("t.utf16.crlf", "rb:utf-16be") {|f|
        content = f.read
        assert_equal("\0a\0\r\0\n\0b\0\r\0\n".force_encoding("UTF-16BE"),
                     content)
      }
    }
  end

  def test_read_newline_conversion_error
    with_tmpdir {
      generate_file("empty.txt", "")
      # ascii incompatible encoding without conversion needs binmode.
      assert_raise(ArgumentError) {
        open("empty.txt", "rt:utf-16be") {|f| }
      }
      assert_raise(ArgumentError) {
        open("empty.txt", "r:utf-16be") {|f| }
      }
    }
  end

  def test_read_mode
    with_tmpdir {
      generate_file("t", "a\rb\r\nc\n\xc2\xa2")
      generate_file("ie", "a\rb\r\nc\n\e$B\x42\x22\e(B")
      generate_file("iu", "a\rb\r\nc\n\e$B\x21\x71\e(B")
      generate_file("be", "\0a\0\r\0b\0\r\0\n\0c\0\n\x85\x35")
      generate_file("bu", "\0a\0\r\0b\0\r\0\n\0c\0\n\0\xa2")
      # "\xc2\xa2" is valid as EUC-JP and UTF-8
      #   EUC-JP        UTF-8           Unicode
      #   0xC2A2        0xE894B5        U+8535
      #   0xA1F1        0xC2A2          U+00A2

      open("t","rt") {|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding(Encoding.default_external), f.read) }
      open("t","rb") {|f| assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding(Encoding::ASCII_8BIT), f.read) }

      open("t","rt:euc-jp") {|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("EUC-JP"), f.read) }
      open("t","rb:euc-jp") {|f| assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("EUC-JP"), f.read) }
      open("t","rt:utf-8") {|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("UTF-8"), f.read) }
      open("t","rb:utf-8") {|f| assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("UTF-8"), f.read) }
      assert_raise(ArgumentError) { open("t", "rt:iso-2022-jp") {|f| } }
      open("t","rb:iso-2022-jp") {|f| assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("ISO-2022-JP"), f.read) }

      open("t","rt:euc-jp:utf-8") {|f| assert_equal("a\nb\nc\n\u8535", f.read) }
      open("t","rt:utf-8:euc-jp") {|f| assert_equal("a\nb\nc\n\xa1\xf1".force_encoding("EUC-JP"), f.read) }
      open("t","rb:euc-jp:utf-8") {|f| assert_equal("a\rb\r\nc\n\u8535", f.read) }
      open("t","rb:utf-8:euc-jp") {|f| assert_equal("a\rb\r\nc\n\xa1\xf1".force_encoding("EUC-JP"), f.read) }

      open("t","rt:euc-jp:iso-2022-jp"){|f| assert_equal("a\nb\nc\n\e$B\x42\x22\e(B".force_encoding("ISO-2022-JP"), f.read) }
      open("t","rt:utf-8:iso-2022-jp"){|f| assert_equal("a\nb\nc\n\e$B\x21\x71\e(B".force_encoding("ISO-2022-JP"), f.read) }
      open("t","rt:euc-jp:utf-16be"){|f| assert_equal("\0a\0\n\0b\0\n\0c\0\n\x85\x35".force_encoding("UTF-16BE"), f.read) }
      open("t","rt:utf-8:utf-16be"){|f| assert_equal("\0a\0\n\0b\0\n\0c\0\n\0\xa2".force_encoding("UTF-16BE"), f.read) }
      open("t","rb:euc-jp:iso-2022-jp"){|f|assert_equal("a\rb\r\nc\n\e$B\x42\x22\e(B".force_encoding("ISO-2022-JP"),f.read)}
      open("t","rb:utf-8:iso-2022-jp"){|f|assert_equal("a\rb\r\nc\n\e$B\x21\x71\e(B".force_encoding("ISO-2022-JP"),f.read)}
      open("t","rb:euc-jp:utf-16be"){|f|assert_equal("\0a\0\r\0b\0\r\0\n\0c\0\n\x85\x35".force_encoding("UTF-16BE"),f.read)}
      open("t","rb:utf-8:utf-16be"){|f|assert_equal("\0a\0\r\0b\0\r\0\n\0c\0\n\0\xa2".force_encoding("UTF-16BE"),f.read)}

      open("ie","rt:iso-2022-jp:euc-jp"){|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("EUC-JP"), f.read) }
      open("iu","rt:iso-2022-jp:utf-8"){|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("UTF-8"), f.read) }
      open("be","rt:utf-16be:euc-jp"){|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("EUC-JP"), f.read) }
      open("bu","rt:utf-16be:utf-8"){|f| assert_equal("a\nb\nc\n\xc2\xa2".force_encoding("UTF-8"), f.read) }
      open("ie","rb:iso-2022-jp:euc-jp"){|f|assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("EUC-JP"),f.read)}
      open("iu","rb:iso-2022-jp:utf-8"){|f|assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("UTF-8"),f.read)}
      open("be","rb:utf-16be:euc-jp"){|f|assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("EUC-JP"),f.read)}
      open("bu","rb:utf-16be:utf-8"){|f|assert_equal("a\rb\r\nc\n\xc2\xa2".force_encoding("UTF-8"),f.read)}

      open("ie","rt:iso-2022-jp:utf-16be"){|f|assert_equal("\0a\0\n\0b\0\n\0c\0\n\x85\x35".force_encoding("UTF-16BE"),f.read)}
      open("be","rt:utf-16be:iso-2022-jp"){|f|assert_equal("a\nb\nc\n\e$B\x42\x22\e(B".force_encoding("ISO-2022-JP"),f.read)}
      open("ie","rb:iso-2022-jp:utf-16be"){|f|assert_equal("\0a\0\r\0b\0\r\0\n\0c\0\n\x85\x35".force_encoding("UTF-16BE"),f.read)}
      open("be","rb:utf-16be:iso-2022-jp"){|f|assert_equal("a\rb\r\nc\n\e$B\x42\x22\e(B".force_encoding("ISO-2022-JP"),f.read)}
    }
  end

  def assert_write(expected, mode, *args)
    with_tmpdir {
      open("t", mode) {|f|
        args.each {|arg| f.print arg }
      }
      content = File.read("t", :mode=>"rb:ascii-8bit")
      assert_equal(expected.dup.force_encoding("ascii-8bit"),
                   content.force_encoding("ascii-8bit"))
    }
  end

  def test_write_mode
    # "\xc2\xa2" is valid as EUC-JP and UTF-8
    #   EUC-JP        UTF-8           Unicode
    #   0xC2A2        0xE894B5        U+8535
    #   0xA1F1        0xC2A2          U+00A2
    a = "a\rb\r\nc\n"
    e = "\xc2\xa2".force_encoding("euc-jp")
    u8 = "\xc2\xa2".force_encoding("utf-8")
    u16 = "\x85\x35\0\r\x00\xa2\0\r\0\n\0\n".force_encoding("utf-16be")
    i = "\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n".force_encoding("iso-2022-jp")
    n = system_newline
    un = n.encode("utf-16be").force_encoding("ascii-8bit")

    assert_write("a\rb\r#{n}c#{n}", "wt", a)
    assert_write("\xc2\xa2", "wt", e)
    assert_write("\xc2\xa2", "wt", u8)

    assert_write("a\rb\r\nc\n", "wb", a)
    assert_write("\xc2\xa2", "wb", e)
    assert_write("\xc2\xa2", "wb", u8)

    #assert_write("\x85\x35\0\r\x00\xa2\0\r\0\n\0\n", "wt", u16) should raise
    #assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n", "wt", i) should raise
    assert_write("\x85\x35\0\r\x00\xa2\0\r\0\n\0\n", "wb", u16)
    assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n", "wb", i)

    t_write_mode_enc
    t_write_mode_enc(":utf-8")
  end

  def t_write_mode_enc(enc="")
    # "\xc2\xa2" is valid as EUC-JP and UTF-8
    #   EUC-JP        UTF-8           Unicode
    #   0xC2A2        0xE894B5        U+8535
    #   0xA1F1        0xC2A2          U+00A2
    a = "a\rb\r\nc\n"
    e = "\xc2\xa2".force_encoding("euc-jp")
    u8 = "\xc2\xa2".force_encoding("utf-8")
    u16 = "\x85\x35\0\r\x00\xa2\0\r\0\n\0\n".force_encoding("utf-16be")
    i = "\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n".force_encoding("iso-2022-jp")
    n = system_newline
    un = n.encode("utf-16be").force_encoding("ascii-8bit")

    assert_write("a\rb\r#{n}c#{n}", "wt:euc-jp#{enc}", a)
    assert_write("\xc2\xa2", "wt:euc-jp#{enc}", e)
    assert_write("\xa1\xf1", "wt:euc-jp#{enc}", u8)

    assert_write("a\rb\r\nc\n", "wb:euc-jp#{enc}", a)
    assert_write("\xc2\xa2", "wb:euc-jp#{enc}", e)
    assert_write("\xa1\xf1", "wb:euc-jp#{enc}", u8)

    assert_write("\xc2\xa2\r\xa1\xf1\r#{n}#{n}", "wt:euc-jp#{enc}", u16)
    assert_write("\xc2\xa2\r\xa1\xf1\r#{n}#{n}", "wt:euc-jp#{enc}", i)
    assert_write("\xc2\xa2\r\xa1\xf1\r\n\n", "wb:euc-jp#{enc}", u16)
    assert_write("\xc2\xa2\r\xa1\xf1\r\n\n", "wb:euc-jp#{enc}", i)

    assert_write("\0a\0\r\0b\0\r#{un}\0c#{un}", "wt:utf-16be#{enc}", a)
    assert_write("\x85\x35", "wt:utf-16be#{enc}", e)
    assert_write("\x00\xa2", "wt:utf-16be#{enc}", u8)
    assert_write("a\rb\r#{n}c#{n}", "wt:iso-2022-jp#{enc}", a)
    assert_write("\e$B\x42\x22\e(B", "wt:iso-2022-jp#{enc}", e)
    assert_write("\e$B\x21\x71\e(B", "wt:iso-2022-jp#{enc}", u8)

    assert_write("\0a\0\r\0b\0\r\0\n\0c\0\n", "wb:utf-16be#{enc}", a)
    assert_write("\x85\x35", "wb:utf-16be#{enc}", e)
    assert_write("\x00\xa2", "wb:utf-16be#{enc}", u8)
    assert_write("a\rb\r\nc\n", "wb:iso-2022-jp#{enc}", a)
    assert_write("\e$B\x42\x22\e(B", "wb:iso-2022-jp#{enc}", e)
    assert_write("\e$B\x21\x71\e(B", "wb:iso-2022-jp#{enc}", u8)

    assert_write("\x85\x35\0\r\x00\xa2\0\r#{un}#{un}", "wt:utf-16be#{enc}", u16)
    assert_write("\x85\x35\0\r\x00\xa2\0\r#{un}#{un}", "wt:utf-16be#{enc}", i)
    assert_write("\x85\x35\0\r\x00\xa2\0\r\0\n\0\n", "wb:utf-16be#{enc}", u16)
    assert_write("\x85\x35\0\r\x00\xa2\0\r\0\n\0\n", "wb:utf-16be#{enc}", i)
    assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r#{n}#{n}", "wt:iso-2022-jp#{enc}", u16)
    assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r#{n}#{n}", "wt:iso-2022-jp#{enc}", i)
    assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n", "wb:iso-2022-jp#{enc}", u16)
    assert_write("\e$B\x42\x22\e(B\r\e$B\x21\x71\e(B\r\n\n", "wb:iso-2022-jp#{enc}", i)
  end

  def test_write_mode_fail
    return if system_newline == "\n"
    with_tmpdir {
      open("t", "wt") {|f|
        assert_raise(ArgumentError) { f.print "\0\r\0\r\0\n\0\n".force_encoding("utf-16be") }
      }
    }
  end

  def test_write_ascii_incompat
    with_tmpdir {
      open("t.utf8", "wb:utf-8:utf-16be") {|f| }
      open("t.utf8", "wt:utf-8:utf-16be") {|f| }
      open("t.utf8", "w:utf-8:utf-16be") {|f| }
      open("t.utf16", "wb:utf-16be") {|f| }
      open("t.utf16", "wt:utf-16be") {|f| }
      open("t.utf16", "w:utf-16be") {|f| }
    }
  end

  def test_binmode_write_ascii_incompat_internal
    with_tmpdir {
      open("t.utf8.lf", "wb:utf-8:utf-16be") {|f|
        f.print "\0a\0\n\0b\0\n".force_encoding("UTF-16BE")
      }
      content = File.read("t.utf8.lf", :mode=>"rb:ascii-8bit")
      assert_equal("a\nb\n", content)

      open("t.utf8.lf", "wb:utf-16be") {|f|
        f.print "\0a\0\n\0b\0\n".force_encoding("UTF-16BE")
      }
      content = File.read("t.utf8.lf", :mode=>"rb:ascii-8bit")
      assert_equal("\0a\0\n\0b\0\n", content)
    }
  end

  def test_binary
    with_tmpdir {
      src = "a\nb\rc\r\nd\n"
      generate_file("t.txt", src)
      open("t.txt", "rb") {|f|
        assert_equal(src, f.read)
      }
      open("t.txt", "r", :binmode=>true) {|f|
        assert_equal(src, f.read)
      }
      if system_newline == "\n"
        open("t.txt", "r") {|f|
          assert_equal(src, f.read)
        }
      end
    }
  end

  def test_binmode
    with_tmpdir {
      src = "a\r\nb\r\nc\r\n"
      generate_file("t.txt", src)
      open("t.txt", "rt") {|f|
        assert_equal("a", f.getc)
        assert_equal("\n", f.getc)
        f.binmode
        assert_equal("b", f.getc)
        assert_equal("\r", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("c", f.getc)
        assert_equal("\r", f.getc)
        assert_equal("\n", f.getc)
        assert_equal(nil, f.getc)
      }
    }
  end

  def test_binmode2
    with_tmpdir {
      src = "a\r\nb\r\nc\r\n"
      generate_file("t.txt", src)
      open("t.txt", "rt:euc-jp:utf-8") {|f|
        assert_equal("a", f.getc)
        assert_equal("\n", f.getc)
        f.binmode
        assert_equal("b", f.getc)
        assert_equal("\r", f.getc)
        assert_equal("\n", f.getc)
        assert_equal("c", f.getc)
        assert_equal("\r", f.getc)
        assert_equal("\n", f.getc)
        assert_equal(nil, f.getc)
      }
    }
  end

  def test_binmode3
    with_tmpdir {
      src = "\u3042\r\n"
      generate_file("t.txt", src)
      srcbin = src.dup.force_encoding("ascii-8bit")
      open("t.txt", "rt:utf-8:euc-jp") {|f|
        f.binmode
        result = f.read
        assert_str_equal(srcbin, result)
        assert_equal(Encoding::ASCII_8BIT, result.encoding)
      }
    }
  end

  def test_invalid_r
    with_tmpdir {
      generate_file("t.txt", "a\x80b")
      open("t.txt", "r:utf-8:euc-jp", :invalid => :replace) {|f|
        assert_equal("a?b", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :invalid => :replace, :replace => "") {|f|
        assert_equal("ab", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :undef => :replace) {|f|
        assert_raise(Encoding::InvalidByteSequenceError) { f.read }
        assert_equal("b", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :undef => :replace, :replace => "") {|f|
        assert_raise(Encoding::InvalidByteSequenceError) { f.read }
        assert_equal("b", f.read)
      }
    }
  end

  def test_undef_r
    with_tmpdir {
      generate_file("t.txt", "a\uFFFDb")
      open("t.txt", "r:utf-8:euc-jp", :undef => :replace) {|f|
        assert_equal("a?b", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :undef => :replace, :replace => "") {|f|
        assert_equal("ab", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :invalid => :replace) {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.read }
        assert_equal("b", f.read)
      }
      open("t.txt", "r:utf-8:euc-jp", :invalid => :replace, :replace => "") {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.read }
        assert_equal("b", f.read)
      }
    }
  end

  def test_invalid_w
    with_tmpdir {
      invalid_utf8 = "a\x80b".force_encoding("utf-8")
      open("t.txt", "w:euc-jp", :invalid => :replace) {|f|
        assert_nothing_raised { f.write invalid_utf8 }
      }
      assert_equal("a?b", File.read("t.txt"))

      open("t.txt", "w:euc-jp", :invalid => :replace, :replace => "") {|f|
        assert_nothing_raised { f.write invalid_utf8 }
      }
      assert_equal("ab", File.read("t.txt"))

      open("t.txt", "w:euc-jp", :undef => :replace) {|f|
        assert_raise(Encoding::InvalidByteSequenceError) { f.write invalid_utf8 }
      }
      open("t.txt", "w:euc-jp", :undef => :replace, :replace => "") {|f|
        assert_raise(Encoding::InvalidByteSequenceError) { f.write invalid_utf8 }
      }
    }
  end

  def test_undef_w_stateless
    with_tmpdir {
      generate_file("t.txt", "a\uFFFDb")
      open("t.txt", "w:euc-jp:utf-8", :undef => :replace) {|f|
        assert_nothing_raised { f.write "a\uFFFDb" }
      }
      assert_equal("a?b", File.read("t.txt"))
      open("t.txt", "w:euc-jp:utf-8", :undef => :replace, :replace => "") {|f|
        assert_nothing_raised { f.write "a\uFFFDb" }
      }
      assert_equal("ab", File.read("t.txt"))
      open("t.txt", "w:euc-jp:utf-8", :invalid => :replace) {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.write "a\uFFFDb" }
      }
      open("t.txt", "w:euc-jp:utf-8", :invalid => :replace, :replace => "") {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.write "a\uFFFDb" }
      }
    }
  end

  def test_undef_w_stateful
    with_tmpdir {
      generate_file("t.txt", "a\uFFFDb")
      open("t.txt", "w:iso-2022-jp:utf-8", :undef => :replace) {|f|
        assert_nothing_raised { f.write "a\uFFFDb" }
      }
      assert_equal("a?b", File.read("t.txt"))
      open("t.txt", "w:iso-2022-jp:utf-8", :undef => :replace, :replace => "") {|f|
        assert_nothing_raised { f.write "a\uFFFDb" }
      }
      assert_equal("ab", File.read("t.txt"))
      open("t.txt", "w:iso-2022-jp:utf-8", :invalid => :replace) {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.write "a\uFFFDb" }
      }
      open("t.txt", "w:iso-2022-jp:utf-8", :invalid => :replace, :replace => "") {|f|
        assert_raise(Encoding::UndefinedConversionError) { f.write "a\uFFFDb" }
      }
    }
  end

  def test_w_xml_attr
    with_tmpdir {
      open("raw.txt", "wb", xml: :attr) {|f| f.print '&<>"\''; f.puts "\u4E02\u3042" }
      content = File.read("raw.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"&amp;&lt;&gt;&quot;'\u4E02\u3042\n\"".force_encoding("ascii-8bit"), content)

      open("ascii.txt", "wb:us-ascii", xml: :attr) {|f| f.print '&<>"\''; f.puts "\u4E02\u3042" }
      content = File.read("ascii.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"&amp;&lt;&gt;&quot;'&#x4E02;&#x3042;\n\"".force_encoding("ascii-8bit"), content)

      open("iso-2022-jp.txt", "wb:iso-2022-jp", xml: :attr) {|f| f.print '&<>"\''; f.puts "\u4E02\u3042" }
      content = File.read("iso-2022-jp.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"&amp;&lt;&gt;&quot;'&#x4E02;\e$B$\"\e(B\n\"".force_encoding("ascii-8bit"), content)

      open("utf-16be.txt", "wb:utf-16be", xml: :attr) {|f| f.print '&<>"\''; f.puts "\u4E02\u3042" }
      content = File.read("utf-16be.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\0\"\0&\0a\0m\0p\0;\0&\0l\0t\0;\0&\0g\0t\0;\0&\0q\0u\0o\0t\0;\0'\x4E\x02\x30\x42\0\n\0\"".force_encoding("ascii-8bit"), content)

      open("eucjp.txt", "w:euc-jp:utf-8", xml: :attr) {|f|
        f.print "\u4E02" # U+4E02 is 0x3021 in JIS X 0212
      }
      content = File.read("eucjp.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"\x8F\xB0\xA1\"".force_encoding("ascii-8bit"), content)

      open("sjis.txt", "w:sjis:utf-8", xml: :attr) {|f|
        f.print "\u4E02" # U+4E02 is 0x3021 in JIS X 0212
      }
      content = File.read("sjis.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"&#x4E02;\"".force_encoding("ascii-8bit"), content)

      open("iso-2022-jp.txt", "w:iso-2022-jp:utf-8", xml: :attr) {|f|
        f.print "\u4E02" # U+4E02 is 0x3021 in JIS X 0212
      }
      content = File.read("iso-2022-jp.txt", :mode=>"rb:ascii-8bit")
      assert_equal("\"&#x4E02;\"".force_encoding("ascii-8bit"), content)
    }
  end

  def test_strip_bom
    with_tmpdir {
      text = "\uFEFFa"
      %w/UTF-8 UTF-16BE UTF-16LE UTF-32BE UTF-32LE/.each do |name|
        path = '%s-bom.txt' % name
        content = text.encode(name)
        generate_file(path, content)
        result = File.read(path, mode: 'rb:BOM|UTF-8')
        assert_equal(content[1].force_encoding("ascii-8bit"),
                     result.force_encoding("ascii-8bit"))
      end

      bug3407 = '[ruby-core:30641]'
      result = File.read('UTF-8-bom.txt', encoding: 'BOM|UTF-8')
      assert_equal("a", result.force_encoding("ascii-8bit"), bug3407)
    }
  end

  def test_cbuf
    with_tmpdir {
      fn = "tst"
      open(fn, "w") {|f| f.print "foo" }
      open(fn, "r+t") {|f|
        f.ungetc(f.getc)
        assert_raise(IOError, "[ruby-dev:40493]") { f.readpartial(2) }
        assert_raise(IOError) { f.read(2) }
        assert_raise(IOError) { f.each_byte {|c| } }
        assert_raise(IOError) { f.getbyte }
        assert_raise(IOError) { f.ungetbyte(0) }
        assert_raise(IOError) { f.sysread(2) }
        assert_raise(IOError) { IO.copy_stream(f, "tmpout") }
        assert_raise(IOError) { f.sysseek(2) }
      }
      open(fn, "r+t") {|f|
        f.ungetc(f.getc)
        assert_equal("foo", f.read)
      }
    }
  end

  def test_text_mode_ungetc_eof
    with_tmpdir {
      open("ff", "w") {|f| }
      open("ff", "rt") {|f|
        f.ungetc "a"
        assert(!f.eof?, "[ruby-dev:40506] (3)")
      }
    }
  end

  def test_cbuf_select
    pipe("US-ASCII:UTF-8", { :universal_newline => true },
         proc do |w|
           w << "\r\n"
         end,
         proc do |r|
           r.ungetc(r.getc)
           assert_equal([[r],[],[]], IO.select([r], nil, nil, 1))
         end)
  end

  def test_textmode_paragraphmode
    pipe("US-ASCII:UTF-8", { :universal_newline => true },
         proc do |w|
           w << "a\n\n\nc".gsub(/\n/, "\r\n")
           w.close
         end,
         proc do |r|
           assert_equal("a\n\n", r.gets(""))
           assert_equal("c", r.gets(""), "[ruby-core:23723] (18)")
         end)
  end

  def test_textmode_paragraph_binaryread
    pipe("US-ASCII:UTF-8", { :universal_newline => true },
         proc do |w|
           w << "a\n\n\ncdefgh".gsub(/\n/, "\r\n")
           w.close
         end,
         proc do |r|
           assert_equal("a\n\n", r.gets(""))
           assert_equal("c", r.getc)
           assert_equal("defgh", r.readpartial(10))
         end)
  end

  def test_textmode_paragraph_nonasciicompat
    bug3534 = ['[ruby-dev:41803]', '[Bug #3534]']
    r, w = IO.pipe
    [Encoding::UTF_32BE, Encoding::UTF_32LE,
     Encoding::UTF_16BE, Encoding::UTF_16LE,
     Encoding::UTF_8].each do |e|
      r.set_encoding(Encoding::US_ASCII, e)
      wthr = Thread.new{ w.print(bug3534[0], "\n\n\n\n", bug3534[1], "\n") }
      assert_equal((bug3534[0]+"\n\n").encode(e), r.gets(""), bug3534[0])
      assert_equal((bug3534[1]+"\n").encode(e), r.gets(), bug3534[1])
      wthr.join
    end
  end

  def test_binmode_paragraph_nonasciicompat
    bug3534 = ['[ruby-dev:41803]', '[Bug #3534]']
    r, w = IO.pipe
    r.binmode
    w.binmode
    [Encoding::UTF_32BE, Encoding::UTF_32LE,
     Encoding::UTF_16BE, Encoding::UTF_16LE,
     Encoding::UTF_8].each do |e|
      r.set_encoding(Encoding::US_ASCII, e)
      wthr = Thread.new{ w.print(bug3534[0], "\n\n\n\n", bug3534[1], "\n") }
      assert_equal((bug3534[0]+"\n\n").encode(e), r.gets(""), bug3534[0])
      assert_equal((bug3534[1]+"\n").encode(e), r.gets(), bug3534[1])
      wthr.join
    end
  end

  def test_puts_widechar
    bug = '[ruby-dev:42212]'
    pipe(Encoding::ASCII_8BIT,
         proc do |w|
           w.binmode
           w.puts(0x010a.chr(Encoding::UTF_32BE))
           w.puts(0x010a.chr(Encoding::UTF_16BE))
           w.puts(0x0a010000.chr(Encoding::UTF_32LE))
           w.puts(0x0a01.chr(Encoding::UTF_16LE))
           w.close
         end,
         proc do |r|
           r.binmode
           assert_equal("\x00\x00\x01\x0a\n", r.read(5), bug)
           assert_equal("\x01\x0a\n", r.read(3), bug)
           assert_equal("\x00\x00\x01\x0a\n", r.read(5), bug)
           assert_equal("\x01\x0a\n", r.read(3), bug)
           assert_equal("", r.read, bug)
           r.close
         end)
  end

  def test_getc_ascii_only
    bug4557 = '[ruby-core:35630]'
    c = with_tmpdir {
      open("a", "wb") {|f| f.puts "a"}
      open("a", "rt") {|f| f.getc}
    }
    assert(c.ascii_only?, "should be ascii_only #{bug4557}")
  end

  def test_default_mode_on_dosish
    with_tmpdir {
      open("a", "w") {|f| f.write "\n"}
      assert_equal("\r\n", IO.binread("a"))
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_default_mode_on_unix
    with_tmpdir {
      open("a", "w") {|f| f.write "\n"}
      assert_equal("\n", IO.binread("a"))
    }
  end unless /mswin|mingw/ =~ RUBY_PLATFORM

  def test_text_mode
    with_tmpdir {
      open("a", "wb") {|f| f.write "\r\n"}
      assert_equal("\n", open("a", "rt"){|f| f.read})
    }
  end

  def test_binary_mode
    with_tmpdir {
      open("a", "wb") {|f| f.write "\r\n"}
      assert_equal("\r\n", open("a", "rb"){|f| f.read})
    }
  end

  def test_default_stdout_stderr_mode
    with_pipe do |in_r, in_w|
      with_pipe do |out_r, out_w|
        pid = Process.spawn({}, EnvUtil.rubybin, in: in_r, out: out_w, err: out_w)
        in_r.close
        out_w.close
        in_w.write <<-EOS
          STDOUT.puts "abc"
          STDOUT.flush
          STDERR.puts "def"
          STDERR.flush
        EOS
        in_w.close
        Process.wait pid
        assert_equal "abc\r\ndef\r\n", out_r.binmode.read
        out_r.close
      end
    end
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_cr_decorator_on_stdout
    with_pipe do |in_r, in_w|
      with_pipe do |out_r, out_w|
        pid = Process.spawn({}, EnvUtil.rubybin, in: in_r, out: out_w)
        in_r.close
        out_w.close
        in_w.write <<-EOS
          STDOUT.set_encoding('locale', nil, newline: :cr)
          STDOUT.puts "abc"
          STDOUT.flush
        EOS
        in_w.close
        Process.wait pid
        assert_equal "abc\r", out_r.binmode.read
        out_r.close
      end
    end
  end

  def test_lf_decorator_on_stdout
    with_pipe do |in_r, in_w|
      with_pipe do |out_r, out_w|
        pid = Process.spawn({}, EnvUtil.rubybin, in: in_r, out: out_w)
        in_r.close
        out_w.close
        in_w.write <<-EOS
          STDOUT.set_encoding('locale', nil, newline: :lf)
          STDOUT.puts "abc"
          STDOUT.flush
        EOS
        in_w.close
        Process.wait pid
        assert_equal "abc\n", out_r.binmode.read
        out_r.close
      end
    end
  end

  def test_crlf_decorator_on_stdout
    with_pipe do |in_r, in_w|
      with_pipe do |out_r, out_w|
        pid = Process.spawn({}, EnvUtil.rubybin, in: in_r, out: out_w)
        in_r.close
        out_w.close
        in_w.write <<-EOS
          STDOUT.set_encoding('locale', nil, newline: :crlf)
          STDOUT.puts "abc"
          STDOUT.flush
        EOS
        in_w.close
        Process.wait pid
        assert_equal "abc\r\n", out_r.binmode.read
        out_r.close
      end
    end
  end

  def test_binmode_with_pipe
    with_pipe do |r, w|
      src = "a\r\nb\r\nc\r\n"
      w.binmode.write src
      w.close

      assert_equal("a", r.getc)
      assert_equal("\n", r.getc)
      r.binmode
      assert_equal("b", r.getc)
      assert_equal("\r", r.getc)
      assert_equal("\n", r.getc)
      assert_equal("c", r.getc)
      assert_equal("\r", r.getc)
      assert_equal("\n", r.getc)
      assert_equal(nil, r.getc)
      r.close
    end
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_stdin_binmode
    with_pipe do |in_r, in_w|
      with_pipe do |out_r, out_w|
        pid = Process.spawn({}, EnvUtil.rubybin, '-e', <<-'End', in: in_r, out: out_w)
          STDOUT.binmode
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDIN.binmode
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
          STDOUT.write STDIN.getc
        End
        in_r.close
        out_w.close
        src = "a\r\nb\r\nc\r\n"
        in_w.binmode.write src
        in_w.close
        Process.wait pid
        assert_equal "a\nb\r\nc\r\n", out_r.binmode.read
        out_r.close
      end
    end
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_read_with_length
    with_tmpdir {
      str = "a\nb"
      generate_file("tmp", str)
      open("tmp", "r") do |f|
        assert_equal(str, f.read(3))
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_read_with_length_binmode
    with_tmpdir {
      str = "a\r\nb\r\nc\r\n\r\n"
      generate_file("tmp", str)
      open("tmp", "r") do |f|
        # read with length should be binary mode
        assert_equal("a\r\n", f.read(3)) # binary
        assert_equal("b\nc\n\n", f.read) # text
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_gets_and_read_with_binmode
    with_tmpdir {
      str = "a\r\nb\r\nc\r\n\n\r\n"
      generate_file("tmp", str)
      open("tmp", "r") do |f|
        assert_equal("a\n", f.gets)      # text
        assert_equal("b\r\n", f.read(3)) # binary
        assert_equal("c\r\n", f.read(3)) # binary
        assert_equal("\n\n", f.read)     # text
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_getc_and_read_with_binmode
    with_tmpdir {
      str = "a\r\nb\r\nc\n\n\r\n\r\n"
      generate_file("tmp", str)
      open("tmp", "r") do |f|
        assert_equal("a", f.getc)         # text
        assert_equal("\n", f.getc)        # text
        assert_equal("b\r\n", f.read(3))  # binary
        assert_equal("c\n\n\n\n", f.read) # text
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_read_with_binmode_and_gets
    with_tmpdir {
      str = "a\r\nb\r\nc\r\n\r\n"
      open("tmp", "wb") { |f| f.write str }
      open("tmp", "r") do |f|
        assert_equal("a", f.getc)         # text
        assert_equal("\n", f.getc)        # text
        assert_equal("b\r\n", f.read(3))  # binary
        assert_equal("c\n", f.gets)       # text
        assert_equal("\n", f.gets)        # text
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_read_with_binmode_and_getc
    with_tmpdir {
      str = "a\r\nb\r\nc\r\n\r\n"
      open("tmp", "wb") { |f| f.write str }
      open("tmp", "r") do |f|
        assert_equal("a", f.getc)         # text
        assert_equal("\n", f.getc)        # text
        assert_equal("b\r\n", f.read(3))  # binary
        assert_equal("c", f.getc)         # text
        assert_equal("\n", f.getc)        # text
        assert_equal("\n", f.getc)        # text
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_read_write_with_binmode
    with_tmpdir {
      str = "a\r\n"
      generate_file("tmp", str)
      open("tmp", "r+") do |f|
        assert_equal("a\r\n", f.read(3))  # binary
        f.write("b\n\n");                 # text
        f.rewind
        assert_equal("a\nb\n\n", f.read)  # text
        f.rewind
        assert_equal("a\r\nb\r\n\r\n", f.binmode.read) # binary
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_seek_with_setting_binmode
    with_tmpdir {
      str = "a\r\nb\r\nc\r\n\r\n\n\n\n\n\n\n\n"
      generate_file("tmp", str)
      open("tmp", "r") do |f|
        assert_equal("a\n", f.gets)      # text
        assert_equal("b\r\n", f.read(3)) # binary
      end
    }
  end if /mswin|mingw/ =~ RUBY_PLATFORM

  def test_error_nonascii
    bug6071 = '[ruby-dev:45279]'
    paths = ["\u{3042}".encode("sjis"), "\u{ff}".encode("iso-8859-1")]
    encs = with_tmpdir {
      paths.map {|path|
        open(path) rescue $!.message.encoding
      }
    }
    assert_equal(paths.map(&:encoding), encs, bug6071)
  end

  def test_inspect_nonascii
    bug6072 = '[ruby-dev:45280]'
    paths = ["\u{3042}".encode("sjis"), "\u{ff}".encode("iso-8859-1")]
    encs = with_tmpdir {
      paths.map {|path|
        open(path, "wb") {|f| f.inspect.encoding}
      }
    }
    assert_equal(paths.map(&:encoding), encs, bug6072)
  end
end
