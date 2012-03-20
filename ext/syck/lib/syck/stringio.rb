warn "#{caller[0]}: yaml/stringio is deprecated" if $VERBOSE

#
# Limited StringIO if no core lib is available
#
begin
require 'stringio'
rescue LoadError
    # StringIO based on code by MoonWolf
    class StringIO
        def initialize(string="")
            @string=string
            @pos=0
            @eof=(string.size==0)
        end
        def pos
            @pos
        end
        def eof
            @eof
        end
        alias eof? eof
        def readline(rs=$/)
            if @eof
                raise EOFError
            else
                if p = @string[@pos..-1]=~rs
                    line = @string[@pos,p+1]
                else
                    line = @string[@pos..-1]
                end
                @pos+=line.size
                @eof =true if @pos==@string.size
                $_ = line
            end
        end
        def rewind
            seek(0,0)
        end
        def seek(offset,whence)
            case whence
            when 0
                @pos=offset
            when 1
                @pos+=offset
            when 2
                @pos=@string.size+offset
            end
            @eof=(@pos>=@string.size)
            0
        end
    end

	#
	# Class method for creating streams
	#
	def Syck.make_stream( io )
        if String === io
            io = StringIO.new( io )
        elsif not IO === io
            raise Syck::Error, "YAML stream must be an IO or String object."
        end
        if Syck::unicode
            def io.readline
                Syck.utf_to_internal( readline( @ln_sep ), @utf_encoding )
            end
            def io.check_unicode
                @utf_encoding = Syck.sniff_encoding( read( 4 ) )
                @ln_sep = Syck.enc_separator( @utf_encoding )
                seek( -4, IO::SEEK_CUR )
            end
		    def io.utf_encoding
		    	@utf_encoding
		    end
            io.check_unicode
        else
            def io.utf_encoding
                :None
            end
        end
        io
	end

end

