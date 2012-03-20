# $Id$

module Test
  module Unit
    module FileAssertions

      def _wrap_assertion
        yield
      end

      def assert_same_file(from, to, message=nil)
        _wrap_assertion {
          assert_block("file #{from} != #{to}#{message&&': '}#{message||''}") {
            File.read(from) == File.read(to)
          }
        }
      end

      def assert_same_entry(from, to, message=nil)
        a = File.stat(from)
        b = File.stat(to)
        msg = "#{message&&': '}#{message||''}"
        assert_equal a.mode, b.mode, "mode #{a.mode} != #{b.mode}#{msg}"
        #assert_equal a.atime, b.atime
        assert_equal_timestamp a.mtime, b.mtime, "mtime #{a.mtime} != #{b.mtime}#{msg}"
        assert_equal a.uid, b.uid, "uid #{a.uid} != #{b.uid}#{msg}"
        assert_equal a.gid, b.gid, "gid #{a.gid} != #{b.gid}#{msg}"
      end

      def assert_file_exist(path, message=nil)
        _wrap_assertion {
          assert_block("file not exist: #{path}#{message&&': '}#{message||''}") {
            File.exist?(path)
          }
        }
      end

      def assert_file_not_exist(path, message=nil)
        _wrap_assertion {
          assert_block("file not exist: #{path}#{message&&': '}#{message||''}") {
            not File.exist?(path)
          }
        }
      end

      def assert_directory(path, message=nil)
        _wrap_assertion {
          assert_block("is not directory: #{path}#{message&&': '}#{message||''}") {
            File.directory?(path)
          }
        }
      end

      def assert_symlink(path, message=nil)
        _wrap_assertion {
          assert_block("is not a symlink: #{path}#{message&&': '}#{message||''}") {
            File.symlink?(path)
          }
        }
      end

      def assert_not_symlink(path)
        _wrap_assertion {
          assert_block("is a symlink: #{path}#{message&&': '}#{message||''}") {
            not File.symlink?(path)
          }
        }
      end

      def assert_equal_time(expected, actual, message=nil)
        _wrap_assertion {
	  expected_str = expected.to_s
	  actual_str = actual.to_s
	  if expected_str == actual_str
	    expected_str << " (nsec=#{expected.nsec})"
	    actual_str << " (nsec=#{actual.nsec})"
	  end
	  full_message = build_message(message, <<EOT)
<#{expected_str}> expected but was
<#{actual_str}>.
EOT
	  assert_block(full_message) { expected == actual }
        }
      end

      def assert_equal_timestamp(expected, actual, message=nil)
        _wrap_assertion {
	  expected_str = expected.to_s
	  actual_str = actual.to_s
	  if expected_str == actual_str
	    expected_str << " (nsec=#{expected.nsec})"
	    actual_str << " (nsec=#{actual.nsec})"
	  end
	  full_message = build_message(message, <<EOT)
<#{expected_str}> expected but was
<#{actual_str}>.
EOT
          # subsecond timestamp is not portable.
	  assert_block(full_message) { expected.tv_sec == actual.tv_sec }
        }
      end

    end
  end
end
