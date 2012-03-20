#
#   shell/system-command.rb -
#       $Release Version: 0.7 $
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "shell/filter"

class Shell
  class SystemCommand < Filter
    def initialize(sh, command, *opts)
      if t = opts.find{|opt| !opt.kind_of?(String) && opt.class}
        Shell.Fail Error::TypeError, t.class, "String"
      end
      super(sh)
      @command = command
      @opts = opts

      @input_queue = Queue.new
      @pid = nil

      sh.process_controller.add_schedule(self)
    end

    attr_reader :command
    alias name command

    def wait?
      @shell.process_controller.waiting_job?(self)
    end

    def active?
      @shell.process_controller.active_job?(self)
    end

    def input=(inp)
      super
      if active?
        start_export
      end
    end

    def start
      notify([@command, *@opts].join(" "))

      @pid, @pipe_in, @pipe_out = @shell.process_controller.sfork(self) {
        Dir.chdir @shell.pwd
        $0 = @command
        exec(@command, *@opts)
      }
      if @input
        start_export
      end
      start_import
    end

    def flush
      @pipe_out.flush if @pipe_out and !@pipe_out.closed?
    end

    def terminate
      begin
        @pipe_in.close
      rescue IOError
      end
      begin
        @pipe_out.close
      rescue IOError
      end
    end

    def kill(sig)
      if @pid
        Process.kill(sig, @pid)
      end
    end

    def start_import
      notify "Job(%id) start imp-pipe.", @shell.debug?
      rs = @shell.record_separator unless rs
      _eop = true
      Thread.start {
        begin
          while l = @pipe_in.gets
            @input_queue.push l
          end
          _eop = false
        rescue Errno::EPIPE
          _eop = false
        ensure
          if !ProcessController::USING_AT_EXIT_WHEN_PROCESS_EXIT and _eop
            notify("warn: Process finishing...",
                   "wait for Job[%id] to finish pipe importing.",
                   "You can use Shell#transact or Shell#check_point for more safe execution.")
            redo
          end
          notify "job(%id}) close imp-pipe.", @shell.debug?
          @input_queue.push :EOF
          @pipe_in.close
        end
      }
    end

    def start_export
      notify "job(%id) start exp-pipe.", @shell.debug?
      _eop = true
      Thread.start{
        begin
          @input.each do |l|
            ProcessController::block_output_synchronize do
              @pipe_out.print l
            end
          end
          _eop = false
        rescue Errno::EPIPE, Errno::EIO
          _eop = false
        ensure
          if !ProcessController::USING_AT_EXIT_WHEN_PROCESS_EXIT and _eop
            notify("shell: warn: Process finishing...",
                   "wait for Job(%id) to finish pipe exporting.",
                   "You can use Shell#transact or Shell#check_point for more safe execution.")
            redo
          end
          notify "job(%id) close exp-pipe.", @shell.debug?
          @pipe_out.close
        end
      }
    end

    alias super_each each
    def each(rs = nil)
      while (l = @input_queue.pop) != :EOF
        yield l
      end
    end

    # ex)
    #    if you wish to output:
    #       "shell: job(#{@command}:#{@pid}) close pipe-out."
    #    then
    #       mes: "job(%id) close pipe-out."
    #    yorn: Boolean(@shell.debug? or @shell.verbose?)
    def notify(*opts)
      @shell.notify(*opts) do |mes|
        yield mes if iterator?

        mes.gsub!("%id", "#{@command}:##{@pid}")
        mes.gsub!("%name", "#{@command}")
        mes.gsub!("%pid", "#{@pid}")
        mes
      end
    end
  end
end
