module Syck

	#
	# YAML::Stream -- for emitting many documents
	#
	class Stream

		attr_accessor :documents, :options

		def initialize( opts = {} )
			@options = opts
			@documents = []
		end

        def []( i )
            @documents[ i ]
        end

		def add( doc )
			@documents << doc
		end

		def edit( doc_num, doc )
                  warn "#{caller[0]}: edit is deprecated" if $VERBOSE
			@documents[ doc_num ] = doc
		end

		def emit( io = nil )
            # opts = @options.dup
			# opts[:UseHeader] = true if @documents.length > 1
            out = Syck.emitter
            out.reset( io || io2 = StringIO.new )
            @documents.each { |v|
                v.to_yaml( out )
            }
            io || ( io2.rewind; io2.read )
		end

	end

end
