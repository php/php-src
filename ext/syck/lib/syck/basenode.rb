#
# YAML::BaseNode class
#

module Syck

    #
    # YAML Generic Model container
    #
    module BaseNode

        #
        # Search for YPath entry and return
        # qualified nodes.
        #
        def select( ypath_str )
            warn "#{caller[0]}: select is deprecated" if $VERBOSE
            matches = match_path( ypath_str )

            #
            # Create a new generic view of the elements selected
            #
            if matches
                result = []
                matches.each { |m|
                    result.push m.last
                }
                Syck.transfer( 'seq', result )
            end
        end

        #
        # Search for YPath entry and return
        # transformed nodes.
        #
        def select!( ypath_str )
            warn "#{caller[0]}: select!() is deprecated" if $VERBOSE
            matches = match_path( ypath_str )

            #
            # Create a new generic view of the elements selected
            #
            if matches
                result = []
                matches.each { |m|
                    result.push m.last.transform
                }
                result
            end
        end

        #
        # Search for YPath entry and return a list of
        # qualified paths.
        #
        def search( ypath_str )
            warn "#{caller[0]}: search() is deprecated" if $VERBOSE
            matches = match_path( ypath_str )

            if matches
                matches.collect { |m|
                    path = []
                    m.each_index { |i|
                        path.push m[i] if ( i % 2 ).zero?
                    }
                    "/" + path.compact.join( "/" )
                }
            end
        end

        def at( seg )
            warn "#{caller[0]}: at() is deprecated" if $VERBOSE
            if Hash === @value
                self[seg]
            elsif Array === @value and seg =~ /\A\d+\Z/ and @value[seg.to_i]
                @value[seg.to_i]
            end
        end

        #
        # YPath search returning a complete depth array
        #
        def match_path( ypath_str )
            warn "#{caller[0]}: match_path is deprecated" if $VERBOSE
            require 'syck/ypath'
            matches = []
            YPath.each_path( ypath_str ) do |ypath|
                seg = match_segment( ypath, 0 )
                matches += seg if seg
            end
            matches.uniq
        end

        #
        # Search a node for a single YPath segment
        #
        def match_segment( ypath, depth )
            warn "#{caller[0]}: match_segment is deprecated" if $VERBOSE
            deep_nodes = []
            seg = ypath.segments[ depth ]
            if seg == "/"
                unless String === @value
                    idx = -1
                    @value.collect { |v|
                        idx += 1
                        if Hash === @value
                            match_init = [v[0].transform, v[1]]
                            match_deep = v[1].match_segment( ypath, depth )
                        else
                            match_init = [idx, v]
                            match_deep = v.match_segment( ypath, depth )
                        end
                        if match_deep
                            match_deep.each { |m|
                                deep_nodes.push( match_init + m )
                            }
                        end
                    }
                end
                depth += 1
                seg = ypath.segments[ depth ]
            end
            match_nodes =
                case seg
                when "."
                    [[nil, self]]
                when ".."
                    [["..", nil]]
                when "*"
                    if @value.is_a? Enumerable
                        idx = -1
                        @value.collect { |h|
                            idx += 1
                            if Hash === @value
                                [h[0].transform, h[1]]
                            else
                                [idx, h]
                            end
                        }
                    end
                else
                    if seg =~ /^"(.*)"$/
                        seg = $1
                    elsif seg =~ /^'(.*)'$/
                        seg = $1
                    end
                    if ( v = at( seg ) )
                        [[ seg, v ]]
                    end
                end
            return deep_nodes unless match_nodes
            pred = ypath.predicates[ depth ]
            if pred
                case pred
                when /^\.=/
                    pred = $'   # '
                    match_nodes.reject! { |n|
                        n.last.value != pred
                    }
                else
                    match_nodes.reject! { |n|
                        n.last.at( pred ).nil?
                    }
                end
            end
            return match_nodes + deep_nodes unless ypath.segments.length > depth + 1

            #puts "DEPTH: #{depth + 1}"
            deep_nodes = []
            match_nodes.each { |n|
                if n[1].is_a? BaseNode
                    match_deep = n[1].match_segment( ypath, depth + 1 )
                    if match_deep
                        match_deep.each { |m|
                            deep_nodes.push( n + m )
                        }
                    end
                else
                    deep_nodes = []
                end
            }
            deep_nodes = nil if deep_nodes.length == 0
            deep_nodes
        end

        #
        # We want the node to act like as Hash
        # if it is.
        #
        def []( *key )
            if Hash === @value
                v = @value.detect { |k,| k.transform == key.first }
                v[1] if v
            elsif Array === @value
                @value.[]( *key )
            end
        end

        def children
            if Hash === @value
                @value.values.collect { |c| c[1] }
            elsif Array === @value
                @value
            end
        end

        def children_with_index
            warn "#{caller[0]}: children_with_index is deprecated, use children" if $VERBOSE
            if Hash === @value
                @value.keys.collect { |i| [self[i], i] }
            elsif Array === @value
                i = -1; @value.collect { |v| i += 1; [v, i] }
            end
        end

        def emit
            transform.to_yaml
        end
    end

end

