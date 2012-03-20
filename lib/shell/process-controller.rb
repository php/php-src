#
#   shell/process-controller.rb -
#       $Release Version: 0.7 $
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require "forwardable"

require "thread"
require "sync"

class Shell
  class ProcessController

    @ProcessControllers = {}
    @ProcessControllersMonitor = Mutex.new
    @ProcessControllersCV = ConditionVariable.new

    @BlockOutputMonitor = Mutex.new
    @BlockOutputCV = ConditionVariable.new

    class << self
      extend Forwardable

      def_delegator("@ProcessControllersMonitor",
                    "synchronize", "process_controllers_exclusive")

      def active_process_controllers
        process_controllers_exclusive do
          @ProcessControllers.dup
        end
      end

      def activate(pc)
        process_controllers_exclusive do
          @ProcessControllers[pc] ||= 0
          @ProcessControllers[pc] += 1
        end
      end

      def inactivate(pc)
        process_controllers_exclusive do
          if @ProcessControllers[pc]
            if (@ProcessControllers[pc] -= 1) == 0
              @ProcessControllers.delete(pc)
              @ProcessControllersCV.signal
            end
          end
        end
      end

      def each_active_object
        process_controllers_exclusive do
          for ref in @ProcessControllers.keys
            yield ref
          end
        end
      end

      def block_output_synchronize(&b)
        @BlockOutputMonitor.synchronize(&b)
      end

      def wait_to_finish_all_process_controllers
        process_controllers_exclusive do
          while !@ProcessControllers.empty?
            Shell::notify("Process finishing, but active shell exists",
                          "You can use Shell#transact or Shell#check_point for more safe execution.")
            if Shell.debug?
              for pc in @ProcessControllers.keys
                Shell::notify(" Not finished jobs in "+pc.shell.to_s)
                for com in pc.jobs
                  com.notify("  Jobs: %id")
                end
              end
            end
            @ProcessControllersCV.wait(@ProcessControllersMonitor)
          end
        end
      end
    end

    # for shell-command complete finish at this process exit.
    USING_AT_EXIT_WHEN_PROCESS_EXIT = true
    at_exit do
      wait_to_finish_all_process_controllers unless $@
    end

    def initialize(shell)
      @shell = shell
      @waiting_jobs = []
      @active_jobs = []
      @jobs_sync = Sync.new

      @job_monitor = Mutex.new
      @job_condition = ConditionVariable.new
    end

    attr_reader :shell

    def jobs
      jobs = []
      @jobs_sync.synchronize(:SH) do
        jobs.concat @waiting_jobs
        jobs.concat @active_jobs
      end
      jobs
    end

    def active_jobs
      @active_jobs
    end

    def waiting_jobs
      @waiting_jobs
    end

    def jobs_exist?
      @jobs_sync.synchronize(:SH) do
        @active_jobs.empty? or @waiting_jobs.empty?
      end
    end

    def active_jobs_exist?
      @jobs_sync.synchronize(:SH) do
        @active_jobs.empty?
      end
    end

    def waiting_jobs_exist?
      @jobs_sync.synchronize(:SH) do
        @waiting_jobs.empty?
      end
    end

    # schedule a command
    def add_schedule(command)
      @jobs_sync.synchronize(:EX) do
        ProcessController.activate(self)
        if @active_jobs.empty?
          start_job command
        else
          @waiting_jobs.push(command)
        end
      end
    end

    # start a job
    def start_job(command = nil)
      @jobs_sync.synchronize(:EX) do
        if command
          return if command.active?
          @waiting_jobs.delete command
        else
          command = @waiting_jobs.shift
#         command.notify "job(%id) pre-start.", @shell.debug?

          return unless command
        end
        @active_jobs.push command
        command.start
#       command.notify "job(%id) post-start.", @shell.debug?

        # start all jobs that input from the job
        for job in @waiting_jobs.dup
          start_job(job) if job.input == command
        end
#       command.notify "job(%id) post2-start.", @shell.debug?
      end
    end

    def waiting_job?(job)
      @jobs_sync.synchronize(:SH) do
        @waiting_jobs.include?(job)
      end
    end

    def active_job?(job)
      @jobs_sync.synchronize(:SH) do
        @active_jobs.include?(job)
      end
    end

    # terminate a job
    def terminate_job(command)
      @jobs_sync.synchronize(:EX) do
        @active_jobs.delete command
        ProcessController.inactivate(self)
        if @active_jobs.empty?
          command.notify("start_job in terminate_job(%id)", Shell::debug?)
          start_job
        end
      end
    end

    # kill a job
    def kill_job(sig, command)
      @jobs_sync.synchronize(:EX) do
        if @waiting_jobs.delete command
          ProcessController.inactivate(self)
          return
        elsif @active_jobs.include?(command)
          begin
            r = command.kill(sig)
            ProcessController.inactivate(self)
          rescue
            print "Shell: Warn: $!\n" if @shell.verbose?
            return nil
          end
          @active_jobs.delete command
          r
        end
      end
    end

    # wait for all jobs to terminate
    def wait_all_jobs_execution
      @job_monitor.synchronize do
        begin
          while !jobs.empty?
            @job_condition.wait(@job_monitor)
            for job in jobs
              job.notify("waiting job(%id)", Shell::debug?)
            end
          end
        ensure
          redo unless jobs.empty?
        end
      end
    end

    # simple fork
    def sfork(command)
      pipe_me_in, pipe_peer_out = IO.pipe
      pipe_peer_in, pipe_me_out = IO.pipe


      pid = nil
      pid_mutex = Mutex.new
      pid_cv = ConditionVariable.new

      Thread.start do
        ProcessController.block_output_synchronize do
          STDOUT.flush
          ProcessController.each_active_object do |pc|
            for jobs in pc.active_jobs
              jobs.flush
            end
          end

          pid = fork {
            Thread.list.each do |th|
#             th.kill unless [Thread.main, Thread.current].include?(th)
              th.kill unless Thread.current == th
            end

            STDIN.reopen(pipe_peer_in)
            STDOUT.reopen(pipe_peer_out)

            ObjectSpace.each_object(IO) do |io|
              if ![STDIN, STDOUT, STDERR].include?(io)
                io.close unless io.closed?
              end
            end

            yield
          }
        end
        pid_cv.signal

        pipe_peer_in.close
        pipe_peer_out.close
        command.notify "job(%name:##{pid}) start", @shell.debug?

        begin
          _pid = nil
          command.notify("job(%id) start to waiting finish.", @shell.debug?)
          _pid = Process.waitpid(pid, nil)
        rescue Errno::ECHILD
          command.notify "warn: job(%id) was done already waitpid."
          _pid = true
          #     rescue
          #       STDERR.puts $!
        ensure
          command.notify("Job(%id): Wait to finish when Process finished.", @shell.debug?)
          # when the process ends, wait until the command terminates
          if USING_AT_EXIT_WHEN_PROCESS_EXIT or _pid
          else
            command.notify("notice: Process finishing...",
                           "wait for Job[%id] to finish.",
                           "You can use Shell#transact or Shell#check_point for more safe execution.")
            redo
          end

#         command.notify "job(%id) pre-pre-finish.", @shell.debug?
          @job_monitor.synchronize do
#           command.notify "job(%id) pre-finish.", @shell.debug?
            terminate_job(command)
#           command.notify "job(%id) pre-finish2.", @shell.debug?
            @job_condition.signal
            command.notify "job(%id) finish.", @shell.debug?
          end
        end
      end

      pid_mutex.synchronize do
        while !pid
          pid_cv.wait(pid_mutex)
        end
      end

      return pid, pipe_me_in, pipe_me_out
    end
  end
end
