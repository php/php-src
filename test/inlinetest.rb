module InlineTest
  def eval_part(libname, sep, part)
    path = libpath(libname)
    program = File.open(path) { |f| f.read }
    mainpart, endpart = program.split(sep)
    if endpart.nil?
      raise RuntimeError.new("No #{part} part in the library '#{path}'")
    end
    eval(endpart, TOPLEVEL_BINDING, path, mainpart.count("\n")+1)
  end
  module_function :eval_part

  def loadtest(libname)
    require(libname)
    in_critical do
      in_progname(libpath(libname)) do
        eval_part(libname, /^(?=if\s+(?:\$0\s*==\s*__FILE__|__FILE__\s*==\s*\$0)(?:[\#\s]|$))/, '$0 == __FILE__')
      end
    end
  end
  module_function :loadtest

  def loadtest__END__part(libname)
    require(libname)
    eval_part(libname, /^__END__\r?$/, '__END__')
  end
  module_function :loadtest__END__part

  @mutex = Mutex.new

  def self.in_critical(&block)
    @mutex.synchronize(&block)
  end

  def self.in_progname(progname)
    $program_name = progname
    alias $0 $program_name
    begin
      yield
    ensure
      alias $0 $PROGRAM_NAME
    end
  end

  def self.libpath(libname)
    libpath = nil
    $:.find do |path|
      File.file?(testname = File.join(path, libname)) && libpath = testname
    end
    if libpath.nil?
      raise RuntimeError.new("'#{libname}' not found")
    end
    libpath
  end
end
