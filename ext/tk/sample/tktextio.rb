#!/usr/bin/env ruby
#
#  TkTextIO class :: handling I/O stream on a TkText widget
#                             by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#  NOTE: TkTextIO supports 'character' (not 'byte') access only.
#        So, for example, TkTextIO#getc returns a character, TkTextIO#pos
#        means the character position, TkTextIO#read(size) counts by
#        characters, and so on.
#        Of course, it is available to make TkTextIO class to suuport
#        'byte' access. However, it may break multi-byte characters.
#        and then, displayed string on the text widget may be garbled.
#        I think that it is not good on the supposed situation of using
#        TkTextIO.
#
require 'tk'
require 'tk/text'
require 'tk/textmark'
require 'thread'

class TkTextIO < TkText
  # keep safe level
  @@create_queues = proc{ [Queue.new, Mutex.new, Queue.new, Mutex.new] }

  OPT_DEFAULTS = {
    'mode'       => nil,
    'overwrite'  => false,
    'text'       => nil,
    'show'       => :pos,
    'wrap'       => 'char',
    'sync'       => true,
    'prompt'     => nil,
    'prompt_cmd' => nil,
    'hist_size'  => 1000,
  }

  def create_self(keys)
    opts = _get_io_params((keys.kind_of?(Hash))? keys: {})

    super(keys)

    @count_var = TkVariable.new

    @write_buffer = ''
    @read_buffer  = ''
    @buf_size = 0
    @buf_max = 1024

    @write_buf_queue, @write_buf_mutex,
    @read_buf_queue,  @read_buf_mutex  = @@create_queues.call

    @idle_flush  = TkTimer.new(:idle, 1, proc{ @flusher.run rescue nil })
    @timer_flush = TkTimer.new(250, -1, proc{ @flusher.run rescue nil })

    @flusher = Thread.new{ loop { Thread.stop; flush() } }

    @receiver = Thread.new{
      begin
        loop {
          str = @write_buf_queue.deq
          @write_buf_mutex.synchronize { @write_buffer << str }
          @idle_flush.start
        }
      ensure
        @flusher.kill
      end
    }

    @timer_flush.start

    _setup_io(opts)
  end
  private :create_self

  def destroy
    @flusher.kill rescue nil

    @idle_flush.stop rescue nil
    @timer_flush.stop rescue nil

    @receiver.kill rescue nil

    super()
  end

  ####################################

  def _get_io_params(keys)
    opts = {}
    self.class.const_get(:OPT_DEFAULTS).each{|k, v|
      if keys.has_key?(k)
        opts[k] = keys.delete(k)
      else
        opts[k] = v
      end
    }
    opts
  end

  def _setup_io(opts)
    unless defined? @txtpos
      @txtpos = TkTextMark.new(self, '1.0')
    else
      @txtpos.set('1.0')
    end
    @txtpos.gravity = :left

    @lineno = 0
    @line_offset = 0

    @hist_max = opts['hist_size'].to_i
    @hist_index = 0
    @history = Array.new(@hist_max)
    @history[0] = ''

    self['wrap'] = wrap

    self.show_mode = opts['show']

    self.value = opts['text'] if opts['text']

    @overwrite = (opts['overwrite'])? true: false

    @sync = opts['sync']

    @prompt = opts['prompt']
    @prompt_cmd = opts['prompt_cmd']

    @open  = {:r => true,  :w => true}  # default is 'r+'

    @console_mode = false
    @end_of_stream = false
    @console_buffer = nil

    case opts['mode']
    when nil
      # do nothing

    when :console, 'console'
      @console_mode = true
      # @console_buffer = TkTextIO.new(:mode=>'r')
      @console_buffer = self.class.new(:mode=>'r')
      self.show_mode = :insert

    when 'r', 'rb'
      @open[:r] = true; @open[:w] = nil

    when 'r+', 'rb+', 'r+b'
      @open[:r] = true; @open[:w] = true

    when 'w', 'wb'
      @open[:r] = nil;  @open[:w] = true
      self.value=''

    when 'w+', 'wb+', 'w+b'
      @open[:r] = true; @open[:w] = true
      self.value=''

    when 'a', 'ab'
      @open[:r] = nil;  @open[:w] = true
      @txtpos.set('end - 1 char')
      @txtpos.gravity = :right

    when 'a+', 'ab+', 'a+b'
      @open[:r] = true;  @open[:w] = true
      @txtpos.set('end - 1 char')
      @txtpos.gravity = :right

    else
      fail ArgumentError, "unknown mode `#{opts['mode']}'"
    end

    unless defined? @ins_head
      @ins_head = TkTextMark.new(self, 'insert')
      @ins_head.gravity = :left
    end

    unless defined? @ins_tail
      @ins_tail = TkTextMark.new(self, 'insert')
      @ins_tail.gravity = :right
    end

    unless defined? @tmp_mark
      @tmp_mark = TkTextMark.new(self, 'insert')
      @tmp_mark.gravity = :left
    end

    if @console_mode
      _set_console_line
      _setup_console_bindings
    end
  end
  private :_get_io_params, :_setup_io

  def _set_console_line
    @tmp_mark.set(@ins_tail)

    mark_set('insert', 'end')

    prompt = ''
    prompt << @prompt_cmd.call if @prompt_cmd
    prompt << @prompt if @prompt
    insert(@tmp_mark, prompt)

    @ins_head.set(@ins_tail)
    @ins_tail.set('insert')

    @txtpos.set(@tmp_mark)

    _see_pos
  end

  def _replace_console_line(str)
    self.delete(@ins_head, @ins_tail)
    self.insert(@ins_head, str)
  end

  def _get_console_line
    @tmp_mark.set(@ins_tail)
    s = self.get(@ins_head, @tmp_mark)
    _set_console_line
    s
  end
  private :_set_console_line, :_replace_console_line, :_get_console_line

  def _cb_up
    @history[@hist_index].replace(self.get(@ins_head, @ins_tail))
    @hist_index += 1
    @hist_index -= 1 if @hist_index >= @hist_max || !@history[@hist_index]
    _replace_console_line(@history[@hist_index]) if @history[@hist_index]
    Tk.callback_break
  end
  def _cb_down
    @history[@hist_index].replace(self.get(@ins_head, @ins_tail))
    @hist_index -= 1
    @hist_index = 0 if @hist_index < 0
    _replace_console_line(@history[@hist_index]) if @history[@hist_index]
    Tk.callback_break
  end
  def _cb_left
    if @console_mode && compare('insert', '<=', @ins_head)
      mark_set('insert', @ins_head)
      Tk.callback_break
    end
  end
  def _cb_backspace
    if @console_mode && compare('insert', '<=', @ins_head)
      Tk.callback_break
    end
  end
  def _cb_ctrl_a
    if @console_mode
      mark_set('insert', @ins_head)
      Tk.callback_break
    end
  end
  def _cb_ctrl_u
    if @console_mode
      mark_set('insert', @ins_head)
      delete('insert', 'insert lineend')
      Tk.callback_break
    end
  end
  private :_cb_up, :_cb_down, :_cb_left, :_cb_backspace,
          :_cb_ctrl_a, :_cb_ctrl_u

  def _setup_console_bindings
    @bindtag = TkBindTag.new

    tags = self.bindtags
    tags[tags.index(self)+1, 0] = @bindtag
    self.bindtags = tags

    @bindtag.bind('Return'){
      insert('end - 1 char', "\n")
      if (str = _get_console_line)
        @read_buf_queue.push(str)

        @history[0].replace(str.chomp)
        @history.pop
        @history.unshift('')
        @hist_index = 0
      end

      Tk.update
      Tk.callback_break
    }
    @bindtag.bind('Alt-Return'){
      Tk.callback_continue
    }

    @bindtag.bind('FocusIn'){
      if @console_mode
        mark_set('insert', @ins_tail)
        Tk.callback_break
      end
    }

    ins_mark = TkTextMark.new(self, 'insert')

    @bindtag.bind('ButtonPress'){
      if @console_mode
        ins_mark.set('insert')
      end
    }

    @bindtag.bind('ButtonRelease-1'){
      if @console_mode && compare('insert', '<=', @ins_head)
        mark_set('insert', ins_mark)
        Tk.callback_break
      end
    }

    @bindtag.bind('ButtonRelease-2', '%x %y'){|x, y|
      if @console_mode
        # paste a text at 'insert' only
        x1, y1, x2, y2 =  bbox(ins_mark)
        unless x == x1 && y == y1
          Tk.event_generate(self, 'ButtonRelease-2', :x=>x1, :y=>y1)
          Tk.callback_break
        end
      end
    }

    @bindtag.bind('Up'){ _cb_up }
    @bindtag.bind('Control-p'){ _cb_up }

    @bindtag.bind('Down'){ _cb_down }
    @bindtag.bind('Control-n'){ _cb_down }

    @bindtag.bind('Left'){ _cb_left }
    @bindtag.bind('Control-b'){ _cb_left }

    @bindtag.bind('BackSpace'){ _cb_backspace }
    @bindtag.bind('Control-h'){ _cb_backspace }

    @bindtag.bind('Home'){ _cb_ctrl_a }
    @bindtag.bind('Control-a'){ _cb_ctrl_a }

    @bindtag.bind('Control-u'){ _cb_ctrl_u }
  end
  private :_setup_console_bindings

  def _block_read(size = nil, ret = '', block_mode = true)
    return '' if size == 0
    return nil if ! @read_buf_queue && @read_buffer.empty?
    ret = '' unless ret.kind_of?(String)
    ret.replace('') unless ret.empty?

    if block_mode == nil # partial
      if @read_buffer.empty?
        ret << @read_buffer.slice!(0..-1)
        return ret
      end
    end

    if size.kind_of?(Numeric)
      loop{
        @read_buf_mutex.synchronize {
          buf_len = @read_buffer.length
          if buf_len >= size
            ret << @read_buffer.slice!(0, size)
            return ret
          else
            ret << @read_buffer.slice!(0..-1)
            size -= buf_len
            return ret unless @read_buf_queue
          end
        }
        @read_buffer << @read_buf_queue.pop
      }
    else # readline
      rs = (size)? size: $/
      rs = rs.to_s if rs.kind_of?(Regexp)
      loop{
        @read_buf_mutex.synchronize {
          if (str = @read_buffer.slice!(/\A(.*)(#{rs})/m))
            ret << str
            return ret
          else
            ret << @read_buffer.slice!(0..-1)
            return ret unless @read_buf_queue
          end
        }
        @read_buffer << @read_buf_queue.pop
      }
    end
  end

  def _block_write
    ###### currently, not support
  end
  private :_block_read, :_block_write

  ####################################

  def <<(obj)
    _write(obj)
    self
  end

  def binmode
    self
  end

  def clone
    fail NotImplementedError, 'cannot clone TkTextIO'
  end
  def dup
    fail NotImplementedError, 'cannot duplicate TkTextIO'
  end

  def close
    close_read
    close_write
    nil
  end
  def close_read
    @open[:r] = false if @open[:r]
    nil
  end
  def close_write
    @open[:w] = false if @opne[:w]
    nil
  end

  def closed?(dir=nil)
    case dir
    when :r, 'r'
      !@open[:r]
    when :w, 'w'
      !@open[:w]
    else
      !@open[:r] && !@open[:w]
    end
  end

  def _check_readable
    fail IOError, "not opened for reading" if @open[:r].nil?
    fail IOError, "closed stream" if !@open[:r]
  end
  def _check_writable
    fail IOError, "not opened for writing" if @open[:w].nil?
    fail IOError, "closed stream" if !@open[:w]
  end
  private :_check_readable, :_check_writable

  def each_line(rs = $/)
    _check_readable
    while(s = self.gets(rs))
      yield(s)
    end
    self
  end
  alias each each_line

  def each_char
    _check_readable
    while(c = self.getc)
      yield(c)
    end
    self
  end
  alias each_byte each_char

  def eof?
    compare(@txtpos, '==', 'end - 1 char')
  end
  alias eof eof?

  def fcntl(*args)
    fail NotImplementedError, "fcntl is not implemented on #{self.class}"
  end

  def fsync
    0
  end

  def fileno
    nil
  end

  def flush
    Thread.pass
    if @open[:w] || ! @write_buffer.empty?
      @write_buf_mutex.synchronize {
        _sync_write_buf(@write_buffer)
        @write_buffer[0..-1] = ''
      }
    end
    self
  end

  def getc
    return _block_read(1) if @console_mode

    _check_readable
    return nil if eof?
    c = get(@txtpos)
    @txtpos.set(@txtpos + '1 char')
    _see_pos
    c
  end

  def gets(rs = $/)
    return _block_read(rs) if @console_mode

    _check_readable
    return nil if eof?
    _readline(rs)
  end

  def ioctrl(*args)
    fail NotImplementedError, 'iocntl is not implemented on TkTextIO'
  end

  def isatty
    false
  end
  def tty?
    false
  end

  def lineno
    @lineno + @line_offset
  end

  def lineno=(num)
    @line_offset = num - @lineno
    num
  end

  def overwrite?
    @overwrite
  end

  def overwrite=(ovwt)
    @overwrite = (ovwt)? true: false
  end

  def pid
    nil
  end

  def index_pos
    index(@txtpos)
  end
  alias tell_index index_pos

  def index_pos=(idx)
    @txtpos.set(idx)
    @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
    _see_pos
    idx
  end

  def pos
    s = get('1.0', @txtpos)
    number(tk_call('string', 'length', s))
  end
  alias tell pos

  def pos=(idx)
    seek(idx, IO::SEEK_SET)
    idx
  end

  def pos_gravity
    @txtpos.gravity
  end

  def pos_gravity=(side)
    @txtpos.gravity = side
    side
  end

  def print(arg=$_, *args)
    _check_writable
    args.unshift(arg)
    args.map!{|val| (val == nil)? 'nil': val.to_s }
    str = args.join($,)
    str << $\ if $\
    _write(str)
    nil
  end
  def printf(*args)
    _check_writable
    _write(sprintf(*args))
    nil
  end

  def putc(c)
    _check_writable
    c = c.chr if c.kind_of?(Fixnum)
    _write(c)
    c
  end

  def puts(*args)
    _check_writable
    if args.empty?
      _write("\n")
      return nil
    end
    args.each{|arg|
      if arg == nil
        _write("nil\n")
      elsif arg.kind_of?(Array)
        puts(*arg)
      elsif arg.kind_of?(String)
        _write(arg.chomp)
        _write("\n")
      else
        begin
          arg = arg.to_ary
          puts(*arg)
        rescue
          puts(arg.to_s)
        end
      end
    }
    nil
  end

  def _read(len)
    epos = @txtpos + "#{len} char"
    s = get(@txtpos, epos)
    @txtpos.set(epos)
    @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
    _see_pos
    s
  end
  private :_read

  def read(len=nil, buf=nil)
    return _block_read(len, buf) if @console_mode

    _check_readable
    if len
      return "" if len == 0
      return nil if eof?
      s = _read(len)
    else
      s = get(@txtpos, 'end - 1 char')
      @txtpos.set('end - 1 char')
      _see_pos
    end
    buf.replace(s) if buf.kind_of?(String)
    s
  end

  def readchar
    return _block_read(1) if @console_mode

    _check_readable
    fail EOFError if eof?
    c = get(@txtpos)
    @txtpos.set(@txtpos + '1 char')
    _see_pos
    c
  end

  def _readline(rs = $/)
    if rs == nil
      s = get(@txtpos, 'end - 1 char')
      @txtpos.set('end - 1 char')
    elsif rs == ''
      @count_var.value  # make it global
      idx = tksearch_with_count([:regexp], @count_var,
                                   "\n(\n)+", @txtpos, 'end - 1 char')
      if idx
        s = get(@txtpos, idx) << "\n"
        @txtpos.set("#{idx} + #{@count_var.value} char")
        @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
      else
        s = get(@txtpos, 'end - 1 char')
        @txtpos.set('end - 1 char')
      end
    else
      @count_var.value  # make it global
      idx = tksearch_with_count(@count_var, rs, @txtpos, 'end - 1 char')
      if idx
        s = get(@txtpos, "#{idx} + #{@count_var.value} char")
        @txtpos.set("#{idx} + #{@count_var.value} char")
        @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
      else
        s = get(@txtpos, 'end - 1 char')
        @txtpos.set('end - 1 char')
      end
    end

    _see_pos
    @lineno += 1
    $_ = s
  end
  private :_readline

  def readline(rs = $/)
    return _block_readline(rs) if @console_mode

    _check_readable
    fail EOFError if eof?
    _readline(rs)
  end

  def readlines(rs = $/)
    if @console_mode
      lines = []
      while (line = _block_readline(rs))
        lines << line
      end
      return lines
    end

    _check_readable
    lines = []
    until(eof?)
      lines << _readline(rs)
    end
    $_ = nil
    lines
  end

  def readpartial(maxlen, buf=nil)
    #return @console_buffer.readpartial(maxlen, buf) if @console_mode
    return _block_read(maxlen, buf, nil) if @console_mode

    _check_readable
    fail EOFError if eof?
    s = _read(maxlen)
    buf.replace(s) if buf.kind_of?(String)
    s
  end

  def reopen(*args)
    fail NotImplementedError, 'reopen is not implemented on TkTextIO'
  end

  def rewind
    @txtpos.set('1.0')
    _see_pos
    @lineno = 0
    @line_offset = 0
    self
  end

  def seek(offset, whence=IO::SEEK_SET)
    case whence
    when IO::SEEK_SET
      offset = "1.0 + #{offset} char" if offset.kind_of?(Numeric)
      @txtpos.set(offset)

    when IO::SEEK_CUR
      offset = "#{offset} char" if offset.kind_of?(Numeric)
      @txtpos.set(@txtpos + offset)

    when IO::SEEK_END
      offset = "#{offset} char" if offset.kind_of?(Numeric)
      @txtpos.set("end - 1 char + #{offset}")

    else
      fail Errno::EINVAL, 'invalid whence argument'
    end

    @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
    _see_pos

    0
  end
  alias sysseek seek

  def _see_pos
    see(@show) if @show
  end
  private :_see_pos

  def show_mode
    (@show == @txtpos)? :pos : @show
  end

  def show_mode=(mode)
    # define show mode  when file position is changed.
    #  mode == :pos or "pos" or true :: see current file position.
    #  mode == :insert or "insert"   :: see insert cursor position.
    #  mode == nil or false          :: do nothing
    #  else see 'mode' position ('mode' should be text index or mark)
    case mode
    when :pos, 'pos', true
      @show = @txtpos
    when :insert, 'insert'
      @show = :insert
    when nil, false
      @show = false
    else
      begin
        index(mode)
      rescue
        fail ArgumentError, 'invalid show-position'
      end
      @show = mode
    end

    _see_pos

    mode
  end

  def stat
    fail NotImplementedError, 'stat is not implemented on TkTextIO'
  end

  def sync
    @sync
  end

  def sync=(mode)
    @sync = mode
  end

  def sysread(len, buf=nil)
    return _block_read(len, buf) if @console_mode

    _check_readable
    fail EOFError if eof?
    s = _read(len)
    buf.replace(s) if buf.kind_of?(String)
    s
  end

  def syswrite(obj)
    _write(obj)
  end

  def to_io
    self
  end

  def trancate(len)
    delete("1.0 + #{len} char", :end)
    0
  end

  def ungetc(c)
    if @console_mode
      @read_buf_mutex.synchronize {
        @read_buffer[0,0] = c.chr
      }
      return nil
    end

    _check_readable
    c = c.chr if c.kind_of?(Fixnum)
    if compare(@txtpos, '>', '1.0')
      @txtpos.set(@txtpos - '1 char')
      delete(@txtpos)
      insert(@txtpos, tk_call('string', 'range', c, 0, 1))
      @txtpos.set(@txtpos - '1 char') if @txtpos.gravity == 'right'
      _see_pos
    else
      fail IOError, 'cannot ungetc at head of stream'
    end
    nil
  end

=begin
  def _write(obj)
    #s = _get_eval_string(obj)
    s = (obj.kind_of?(String))? obj: obj.to_s
    n = number(tk_call('string', 'length', s))
    delete(@txtpos, @txtpos + "#{n} char") if @overwrite
    self.insert(@txtpos, s)
    @txtpos.set(@txtpos + "#{n} char")
    @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)
    _see_pos
    Tk.update if @sync
    n
  end
  private :_write
=end
#=begin
  def _sync_write_buf(s)
    if (n = number(tk_call('string', 'length', s))) > 0
      delete(@txtpos, @txtpos + "#{n} char") if @overwrite
      self.insert(@txtpos, s)
      #Tk.update

      @txtpos.set(@txtpos + "#{n} char")
      @txtpos.set('end - 1 char') if compare(@txtpos, '>=', :end)

      @ins_head.set(@txtpos) if compare(@txtpos, '>', @ins_head)

      _see_pos
    end
    self
  end
  private :_sync_write_buf

  def _write(obj)
    s = (obj.kind_of?(String))? obj: obj.to_s
    n = number(tk_call('string', 'length', s))
    @write_buf_queue.enq(s)
    if @sync
      Thread.pass
      Tk.update
    end
    n
  end
  private :_write
#=end

  def write(obj)
    _check_writable
    _write(obj)
  end
end

####################
#  TEST
####################
if __FILE__ == $0
  ev_loop = Thread.new{Tk.mainloop}

  f = TkFrame.new.pack
  #tio = TkTextIO.new(f, :show=>:nil,
  #tio = TkTextIO.new(f, :show=>:pos,
  tio = TkTextIO.new(f, :show=>:insert,
                     :text=>">>> This is an initial text line. <<<\n\n"){
#    yscrollbar(TkScrollbar.new(f).pack(:side=>:right, :fill=>:y))
    pack(:side=>:left, :fill=>:both, :expand=>true)
  }

  Tk.update

  $stdin  = tio
  $stdout = tio
  $stderr = tio

  STDOUT.print("\n========= TkTextIO#gets for inital text ========\n\n")

  while(s = gets)
    STDOUT.print(s)
  end

  STDOUT.print("\n============ put strings to TkTextIO ===========\n\n")

  puts "On this sample, a text widget works as if it is a I/O stream."
  puts "Please see the code."
  puts
  printf("printf message: %d %X\n", 123456, 255)
  puts
  printf("(output by 'p' method) This TkTextIO object is ...\n")
  p tio
  print(" [ Current wrap mode of this object is 'char'. ]\n")
  puts
  warn("This is a warning message generated by 'warn' method.")
  puts
  puts "current show_mode is #{tio.show_mode}."
  if tio.show_mode == :pos
    puts "So, you can see the current file position on this text widget."
  else
    puts "So, you can see the position '#{tio.show_mode}' on this text widget."
  end
  print("Please scroll up this text widget to see the head of lines.\n")
  print("---------------------------------------------------------\n")

  STDOUT.print("\n=============== TkTextIO#readlines =============\n\n")

  tio.seek(0)
  lines = readlines
  STDOUT.puts(lines.inspect)

  STDOUT.print("\n================== TkTextIO#each ===============\n\n")

  tio.rewind
  tio.each{|line| STDOUT.printf("%2d: %s\n", tio.lineno, line.chomp)}

  STDOUT.print("\n================================================\n\n")

  STDOUT.print("\n========= reverse order (seek by lines) ========\n\n")

  tio.seek(-1, IO::SEEK_END)
  begin
    begin
      tio.seek(:linestart, IO::SEEK_CUR)
    rescue
      # maybe use old version of tk/textmark.rb
      tio.seek('0 char linestart', IO::SEEK_CUR)
    end
    STDOUT.print(gets)
    tio.seek('-1 char linestart -1 char', IO::SEEK_CUR)
  end while(tio.pos > 0)

  STDOUT.print("\n================================================\n\n")

  tio.seek(0, IO::SEEK_END)

  STDOUT.print("tio.sync ==  #{tio.sync}\n")
#  tio.sync = false
#  STDOUT.print("tio.sync ==  #{tio.sync}\n")

  (0..10).each{|i|
    STDOUT.print("#{i}\n")
    s = ''
    (0..1000).each{ s << '*' }
    print(s)
  }
  print("\n")
  print("\n=========================================================\n\n")

  s = ''
  timer = TkTimer.new(:idle, -1, proc{
                        #STDOUT.print("idle call\n")
                        unless s.empty?
                          print(s)
                          s = ''
                        end
                      }).start
  (0..10).each{|i|
    STDOUT.print("#{i}\n")
    (0..1000).each{ s << '*' }
  }
#  timer.stop
  until s.empty?
    sleep 0.1
  end
  timer.stop

=begin
  tio.sync = false
  print("\n")
  #(0..10000).each{ putc('*') }
  (0..10).each{|i|
    STDOUT.print("#{i}\n")
    (0..1000).each{ putc('*') }
  }

  (0..10).each{|i|
    STDOUT.print("#{i}\n")
    s = ''
    (0..1000).each{ s << '*' }
    print(s)
  }
=end

  num = 0
#  io = TkTextIO.new(:mode=>:console, :prompt=>'').pack
#=begin
  io = TkTextIO.new(:mode=>:console,
                    :prompt_cmd=>proc{
                      s = "[#{num}]"
                      num += 1
                      s
                    },
                    :prompt=>'-> ').pack
#=end
  Thread.new{loop{sleep 2; io.puts 'hoge'}}
  Thread.new{loop{p io.gets}}

  ev_loop.join
end
