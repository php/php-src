$expect_verbose = false

class IO
  # Reads from the IO until pattern +pat+ matches or the +timeout+ is over.
  # It returns an array with the read buffer, followed by the matches.
  # If a block is given, the result is yielded to the block and returns nil.
  #
  # The optional timeout parameter defines, in seconds, the total time to wait
  # for the pattern.  If the timeout expires or eof is found, nil is returned
  # or yielded.  However, the buffer in a timeout session is kept for the next
  # expect call.  The default timeout is 9999999 seconds.
  def expect(pat,timeout=9999999)
    buf = ''
    case pat
    when String
      e_pat = Regexp.new(Regexp.quote(pat))
    when Regexp
      e_pat = pat
    else
      raise TypeError, "unsupported pattern class: #{pattern.class}"
    end
    @unusedBuf ||= ''
    while true
      if not @unusedBuf.empty?
        c = @unusedBuf.slice!(0).chr
      elsif !IO.select([self],nil,nil,timeout) or eof? then
        result = nil
        @unusedBuf = buf
        break
      else
        c = getc.chr
      end
      buf << c
      if $expect_verbose
        STDOUT.print c
        STDOUT.flush
      end
      if mat=e_pat.match(buf) then
        result = [buf,*mat.to_a[1..-1]]
        break
      end
    end
    if block_given? then
      yield result
    else
      return result
    end
    nil
  end
end

