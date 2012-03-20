require_relative 'base'

class TestMkmf
  class TestFindExecutable < TestMkmf
    def setup
      super
      @path, ENV["PATH"] = ENV["PATH"], @tmpdir
    end

    def teardown
      ENV["PATH"] = @path
      super
    end

    def test_find_executable
      bug2669 = '[ruby-core:27912]'
      name = "foobar#{$$}#{rand(1000)}"
      exts = mkmf {self.class::CONFIG['EXECUTABLE_EXTS']}.split
      stdout.filter {|s| s.sub(name, "<executable>")}
      exts[0] ||= ""
      exts.each do |ext|
        full = name+ext
        begin
          open(full, "w") {|ff| ff.chmod(0755)}
          result = mkmf {find_executable(name)}
        ensure
          File.unlink(full)
        end
        assert_equal("#{@tmpdir}/#{name}#{ext}", result, bug2669)
      end
    end

    def test_find_executable_dir
      name = "foobar#{$$}#{rand(1000)}"
      exts = mkmf {self.class::CONFIG['EXECUTABLE_EXTS']}.split
      stdout.filter {|s| s.sub(name, "<executable>")}
      exts[0] ||= ""
      exts.each do |ext|
        full = name+ext
        begin
          Dir.mkdir(full)
          result = mkmf {find_executable(name)}
        ensure
          Dir.rmdir(full)
        end
        assert_nil(result)
      end
    end
  end
end
