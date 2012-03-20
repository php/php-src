#
#   loader.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#


module IRB
  class LoadAbort < Exception;end

  module IrbLoader
    @RCS_ID='-$Id$-'

    alias ruby_load load
    alias ruby_require require

    def irb_load(fn, priv = nil)
      path = search_file_from_ruby_path(fn)
      raise LoadError, "No such file to load -- #{fn}" unless path

      load_file(path, priv)
    end

    def search_file_from_ruby_path(fn)
      if /^#{Regexp.quote(File::Separator)}/ =~ fn
	return fn if File.exist?(fn)
	return nil
      end

      for path in $:
	if File.exist?(f = File.join(path, fn))
	  return f
	end
      end
      return nil
    end

    def source_file(path)
      irb.suspend_name(path, File.basename(path)) do
	irb.suspend_input_method(FileInputMethod.new(path)) do
	  |back_io|
	  irb.signal_status(:IN_LOAD) do
	    if back_io.kind_of?(FileInputMethod)
	      irb.eval_input
	    else
	      begin
		irb.eval_input
	      rescue LoadAbort
		print "load abort!!\n"
	      end
	    end
	  end
	end
      end
    end

    def load_file(path, priv = nil)
      irb.suspend_name(path, File.basename(path)) do

	if priv
	  ws = WorkSpace.new(Module.new)
	else
	  ws = WorkSpace.new
	end
	irb.suspend_workspace(ws) do
	  irb.suspend_input_method(FileInputMethod.new(path)) do
	    |back_io|
	    irb.signal_status(:IN_LOAD) do
#	      p irb.conf
	      if back_io.kind_of?(FileInputMethod)
		irb.eval_input
	      else
		begin
		  irb.eval_input
		rescue LoadAbort
		  print "load abort!!\n"
		end
	      end
	    end
	  end
	end
      end
    end

    def old
      back_io = @io
      back_path = @irb_path
      back_name = @irb_name
      back_scanner = @irb.scanner
      begin
 	@io = FileInputMethod.new(path)
 	@irb_name = File.basename(path)
	@irb_path = path
	@irb.signal_status(:IN_LOAD) do
	  if back_io.kind_of?(FileInputMethod)
	    @irb.eval_input
	  else
	    begin
	      @irb.eval_input
	    rescue LoadAbort
	      print "load abort!!\n"
	    end
	  end
	end
      ensure
 	@io = back_io
 	@irb_name = back_name
 	@irb_path = back_path
	@irb.scanner = back_scanner
      end
    end
  end
end

