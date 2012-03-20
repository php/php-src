#!./miniruby
#
#

require 'erb'

class RubyVM
  class Instruction
    def initialize name, opes, pops, rets, comm, body, tvars, sp_inc,
                   orig = self, defopes = [], type = nil,
                   nsc = [], psc = [[], []]

      @name = name
      @opes = opes # [[type, name], ...]
      @pops = pops # [[type, name], ...]
      @rets = rets # [[type, name], ...]
      @comm = comm # {:c => category, :e => en desc, :j => ja desc}
      @body = body # '...'

      @orig    = orig
      @defopes = defopes
      @type    = type
      @tvars   = tvars

      @nextsc = nsc
      @pushsc = psc
      @sc     = []
      @unifs  = []
      @optimized = []
      @is_sc  = false
      @sp_inc = sp_inc
    end

    def add_sc sci
      @sc << sci
      sci.set_sc
    end

    attr_reader :name, :opes, :pops, :rets
    attr_reader :body, :comm
    attr_reader :nextsc, :pushsc
    attr_reader :orig, :defopes, :type
    attr_reader :sc
    attr_reader :unifs, :optimized
    attr_reader :is_sc
    attr_reader :tvars
    attr_reader :sp_inc

    def set_sc
      @is_sc = true
    end

    def add_unif insns
      @unifs << insns
    end

    def add_optimized insn
      @optimized << insn
    end

    def sp_increase_c_expr
      if(pops.any?{|t, v| v == '...'} ||
         rets.any?{|t, v| v == '...'})
        # user definision
        raise "no sp increase definition" if @sp_inc.nil?
        ret = "int inc = 0;\n"

        @opes.each_with_index{|(t, v), i|
          if t == 'rb_num_t' && ((re = /\b#{v}\b/n) =~ @sp_inc ||
                                 @defopes.any?{|t, val| re =~ val})
            ret << "        int #{v} = FIX2INT(opes[#{i}]);\n"
          end
        }
        @defopes.each_with_index{|((t, var), val), i|
          if t == 'rb_num_t' && val != '*' && /\b#{var}\b/ =~ @sp_inc
            ret << "        #{t} #{var} = #{val};\n"
          end
        }

        ret << "        #{@sp_inc};\n"
        ret << "        return depth + inc;"
        ret
      else
        "return depth + #{rets.size - pops.size};"
      end
    end

    def inspect
      "#<Instruction:#{@name}>"
    end
  end

  class InstructionsLoader
    def initialize opts = {}
      @insns    = []
      @insn_map = {}

      @vpath = opts[:VPATH] || File
      @use_const = opts[:use_const]
      @verbose   = opts[:verbose]
      @destdir   = opts[:destdir]

      (@vm_opts = load_vm_opts).each {|k, v|
        @vm_opts[k] = opts[k] if opts.key?(k)
      }

      load_insns_def opts[:"insns.def"] || 'insns.def'

      load_opt_operand_def opts[:"opope.def"] || 'defs/opt_operand.def'
      load_insn_unification_def opts[:"unif.def"] || 'defs/opt_insn_unif.def'
      make_stackcaching_insns if vm_opt?('STACK_CACHING')
    end

    attr_reader :vpath
    attr_reader :destdir

    %w[use_const verbose].each do |attr|
      attr_reader attr
      alias_method "#{attr}?", attr
      remove_method attr
    end

    def [](s)
      @insn_map[s.to_s]
    end

    def each
      @insns.each{|insn|
        yield insn
      }
    end

    def size
      @insns.size
    end

    ###
    private

    def vm_opt? name
      @vm_opts[name]
    end

    def load_vm_opts file = nil
      file ||= 'vm_opts.h'
      opts = {}
      vpath.open(file) do |f|
        f.grep(/^\#define\s+OPT_([A-Z_]+)\s+(\d+)/) do
          opts[$1] = !$2.to_i.zero?
        end
      end
      opts
    end

    SKIP_COMMENT_PATTERN = Regexp.compile(Regexp.escape('/** ##skip'))

    include Enumerable

    def add_insn insn
      @insns << insn
      @insn_map[insn.name] = insn
    end

    def make_insn name, opes, pops, rets, comm, body, sp_inc
      add_insn Instruction.new(name, opes, pops, rets, comm, body, [], sp_inc)
    end

    # str -> [[type, var], ...]
    def parse_vars line
      raise unless /\((.*?)\)/ =~ line
      vars = $1.split(',')
      vars.map!{|v|
        if /\s*(\S+)\s+(\S+)\s*/ =~ v
          type = $1
          var  = $2
        elsif /\s*\.\.\.\s*/ =~ v
          type = var  = '...'
        else
          raise
        end
        [type, var]
      }
      vars
    end

    def parse_comment comm
      c = 'others'
      j = ''
      e = ''
      comm.each_line{|line|
        case line
        when /@c (.+)/
          c = $1
        when /@e (.+)/
          e = $1
        when /@e\s*$/
          e = ''
        when /@j (.+)$/
          j = $1
        when /@j\s*$/
          j = ''
        end
      }
      { :c => c,
        :e => e,
        :j => j,
      }
    end

    def load_insns_def file
      body = insn = opes = pops = rets = nil
      comment = ''

      vpath.open(file) {|f|
        f.instance_variable_set(:@line_no, 0)
        class << f
          def line_no
            @line_no
          end
          def gets
            @line_no += 1
            super
          end
        end

        while line = f.gets
          line.chomp!
          case line

          when SKIP_COMMENT_PATTERN
            while line = f.gets.chomp
              if /\s+\*\/$/ =~ line
                break
              end
            end

            # collect instruction comment
          when /^\/\*\*$/
            while line = f.gets
              if /\s+\*\/\s*$/ =~ line
                break
              else
                comment << line
              end
            end

            # start instruction body
          when /^DEFINE_INSN$/
            insn = f.gets.chomp
            opes = parse_vars(f.gets.chomp)
            pops = parse_vars(f.gets.chomp).reverse
            rets_str = f.gets.chomp
            rets = parse_vars(rets_str).reverse
            comment = parse_comment(comment)
            insn_in = true
            body    = ''

            sp_inc = rets_str[%r"//\s*(.+)", 1]

            raise unless /^\{$/ =~ f.gets.chomp
            line_no = f.line_no

          # end instruction body
          when /^\}/
            if insn_in
              body.instance_variable_set(:@line_no, line_no)
              body.instance_variable_set(:@file, f.path)
              insn = make_insn(insn, opes, pops, rets, comment, body, sp_inc)
              insn_in = false
              comment = ''
            end

          else
            if insn_in
              body << line + "\n"
            end
          end
        end
      }
    end

    ## opt op
    def load_opt_operand_def file
      vpath.foreach(file) {|line|
        line = line.gsub(/\#.*/, '').strip
        next  if line.length == 0
        break if /__END__/ =~ line
        /(\S+)\s+(.+)/ =~ line
        insn = $1
        opts = $2
        add_opt_operand insn, opts.split(/,/).map{|e| e.strip}
      } if file
    end

    def label_escape label
      label.gsub(/\(/, '_O_').
      gsub(/\)/, '_C_').
      gsub(/\*/, '_WC_')
    end

    def add_opt_operand insn_name, opts
      insn = @insn_map[insn_name]
      opes = insn.opes

      if opes.size != opts.size
        raise "operand size mismatcvh for #{insn.name} (opes: #{opes.size}, opts: #{opts.size})"
      end

      ninsn = insn.name + '_OP_' + opts.map{|e| label_escape(e)}.join('_')
      nopes = []
      defv  = []

      opts.each_with_index{|e, i|
        if e == '*'
          nopes << opes[i]
        end
        defv  << [opes[i], e]
      }

      make_insn_operand_optimized(insn, ninsn, nopes, defv)
    end

    def make_insn_operand_optimized orig_insn, name, opes, defopes
      comm = orig_insn.comm.dup
      comm[:c] = 'optimize'
      add_insn insn = Instruction.new(
        name, opes, orig_insn.pops, orig_insn.rets, comm,
        orig_insn.body, orig_insn.tvars, orig_insn.sp_inc,
        orig_insn, defopes)
      orig_insn.add_optimized insn
    end

    ## insn unif
    def load_insn_unification_def file
      vpath.foreach(file) {|line|
        line = line.gsub(/\#.*/, '').strip
        next  if line.length == 0
        break if /__END__/ =~ line
        make_unified_insns line.split.map{|e|
          raise "unknown insn: #{e}" unless @insn_map[e]
          @insn_map[e]
        }
      } if file
    end

    def all_combination sets
      ret = sets.shift.map{|e| [e]}

      sets.each{|set|
        prev = ret
        ret  = []
        prev.each{|ary|
          set.each{|e|
            eary = ary.dup
            eary << e
            ret  << eary
          }
        }
      }
      ret
    end

    def make_unified_insns insns
      if vm_opt?('UNIFY_ALL_COMBINATION')
        insn_sets = insns.map{|insn|
          [insn] + insn.optimized
        }

        all_combination(insn_sets).each{|insns_set|
          make_unified_insn_each insns_set
        }
      else
        make_unified_insn_each insns
      end
    end

    def mk_private_val vals, i, redef
      vals.dup.map{|v|
        # v[0] : type
        # v[1] : var name

        v = v.dup
        if v[0] != '...'
          redef[v[1]] = v[0]
          v[1] = "#{v[1]}_#{i}"
        end
        v
      }
    end

    def mk_private_val2 vals, i, redef
      vals.dup.map{|v|
        # v[0][0] : type
        # v[0][1] : var name
        # v[1] : default val

        pv = v.dup
        v = pv[0] = pv[0].dup
        if v[0] != '...'
          redef[v[1]] = v[0]
          v[1] = "#{v[1]}_#{i}"
        end
        pv
      }
    end

    def make_unified_insn_each insns
      names = []
      opes = []
      pops = []
      rets = []
      comm = {
        :c => 'optimize',
        :e => 'unified insn',
        :j => 'unified insn',
      }
      body = ''
      passed = []
      tvars = []
      defopes = []
      sp_inc = ''

      insns.each_with_index{|insn, i|
        names << insn.name
        redef_vars = {}

        e_opes = mk_private_val(insn.opes, i, redef_vars)
        e_pops = mk_private_val(insn.pops, i, redef_vars)
        e_rets = mk_private_val(insn.rets, i, redef_vars)
        # ToDo: fix it
        e_defs = mk_private_val2(insn.defopes, i, redef_vars)

        passed_vars = []
        while pvar = e_pops.pop
          rvar = rets.pop
          if rvar
            raise "unsupported unif insn: #{insns.inspect}" if rvar[0] == '...'
            passed_vars << [pvar, rvar]
            tvars << rvar
          else
            e_pops.push pvar
            break
          end
        end

        opes.concat e_opes
        pops.concat e_pops
        rets.concat e_rets
        defopes.concat e_defs
        sp_inc += "#{insn.sp_inc}"

        body += "{ /* unif: #{i} */\n" +
                passed_vars.map{|rpvars|
                  pv = rpvars[0]
                  rv = rpvars[1]
                  "#define #{pv[1]} #{rv[1]}"
                }.join("\n") +
                "\n" +
                redef_vars.map{|v, type|
                  "#define #{v} #{v}_#{i}"
                }.join("\n") + "\n" +
                insn.body +
                passed_vars.map{|rpvars|
                  "#undef #{rpvars[0][1]}"
                }.join("\n") +
                "\n" +
                redef_vars.keys.map{|v|
                  "#undef  #{v}"
                }.join("\n") +
                "\n}\n"
      }

      tvars_ary = []
      tvars.each{|tvar|
        unless opes.any?{|var|
          var[1] == tvar[1]
        } || defopes.any?{|pvar|
          pvar[0][1] == tvar[1]
        }
          tvars_ary << tvar
        end
      }
      add_insn insn = Instruction.new("UNIFIED_" + names.join('_'),
                                   opes, pops, rets.reverse, comm, body,
                                   tvars_ary, sp_inc)
      insn.defopes.replace defopes
      insns[0].add_unif [insn, insns]
    end

    ## sc
    SPECIAL_INSN_FOR_SC_AFTER = {
      /\Asend/      => [:a],
      /\Aend/       => [:a],
      /\Ayield/     => [:a],
      /\Aclassdef/  => [:a],
      /\Amoduledef/ => [:a],
    }
    FROM_SC = [[], [:a], [:b], [:a, :b], [:b, :a]]

    def make_stackcaching_insns
      pops = rets = nil

      @insns.dup.each{|insn|
        opops = insn.pops
        orets = insn.rets
        oopes = insn.opes
        ocomm = insn.comm
        oname = insn.name

        after = SPECIAL_INSN_FOR_SC_AFTER.find {|k, v| k =~ oname}

        insns = []
        FROM_SC.each{|from|
          name, pops, rets, pushs1, pushs2, nextsc =
          *calc_stack(insn, from, after, opops, orets)

          make_insn_sc(insn, name, oopes, pops, rets, [pushs1, pushs2], nextsc)
        }
      }
    end

    def make_insn_sc orig_insn, name, opes, pops, rets, pushs, nextsc
      comm = orig_insn.comm.dup
      comm[:c] = 'optimize(sc)'

      scinsn = Instruction.new(
        name, opes, pops, rets, comm,
        orig_insn.body, orig_insn.tvars, orig_insn.sp_inc,
        orig_insn, orig_insn.defopes, :sc, nextsc, pushs)

      add_insn scinsn
      orig_insn.add_sc scinsn
    end

    def self.complement_name st
      "#{st[0] ? st[0] : 'x'}#{st[1] ? st[1] : 'x'}"
    end

    def add_stack_value st
      len = st.length
      if len == 0
        st[0] = :a
        [nil, :a]
      elsif len == 1
        if st[0] == :a
          st[1] = :b
        else
          st[1] = :a
        end
        [nil, st[1]]
      else
        st[0], st[1] = st[1], st[0]
        [st[1], st[1]]
      end
    end

    def calc_stack insn, ofrom, oafter, opops, orets
      from = ofrom.dup
      pops = opops.dup
      rets = orets.dup
      rest_scr = ofrom.dup

      pushs_before = []
      pushs= []

      pops.each_with_index{|e, i|
        if e[0] == '...'
          pushs_before = from
          from = []
        end
        r = from.pop
        break unless r
        pops[i] = pops[i].dup << r
      }

      if oafter
        from = oafter
        from.each_with_index{|r, i|
          rets[i] = rets[i].dup << r if rets[i]
        }
      else
        rets = rets.reverse
        rets.each_with_index{|e, i|
          break if e[0] == '...'
          pushed, r = add_stack_value from
          rets[i] = rets[i].dup << r
          if pushed
            if rest_scr.pop
              pushs << pushed
            end

            if i - 2 >= 0
              rets[i-2].pop
            end
          end
        }
      end

      if false #|| insn.name =~ /test3/
        p ofrom
        p pops
        p rets
        p pushs_before
        p pushs
        p from
        exit
      end

      ret = ["#{insn.name}_SC_#{InstructionsLoader.complement_name(ofrom)}_#{complement_name(from)}",
            pops, rets, pushs_before, pushs, from]
    end
  end

  class SourceCodeGenerator
    def initialize insns
      @insns = insns
    end

    attr_reader :insns

    def generate
      raise "should not reach here"
    end

    def vpath
      @insns.vpath
    end

    def verbose?
      @insns.verbose?
    end

    def use_const?
      @insns.use_const?
    end

    def build_string
      @lines = []
      yield
      @lines.join("\n")
    end

    EMPTY_STRING = ''.freeze

    def commit str = EMPTY_STRING
      @lines << str
    end

    def comment str
      @lines << str if verbose?
    end

    def output_path(fn)
      d = @insns.destdir
      fn = File.join(d, fn) if d
      fn
    end
  end

  ###################################################################
  # vm.inc
  class VmBodyGenerator < SourceCodeGenerator
    # vm.inc
    def generate
      vm_body = ''
      @insns.each{|insn|
        vm_body << "\n"
        vm_body << make_insn_def(insn)
      }
      src = vpath.read('template/vm.inc.tmpl')
      ERB.new(src).result(binding)
    end

    def generate_from_insnname insnname
      make_insn_def @insns[insnname.to_s]
    end

    #######
    private

    def make_header_prepare_stack insn
      comment "  /* prepare stack status */"

      push_ba = insn.pushsc
      raise "unsupport" if push_ba[0].size > 0 && push_ba[1].size > 0

      n = 0
      push_ba.each {|pushs| n += pushs.length}
      commit "  CHECK_STACK_OVERFLOW(REG_CFP, #{n});" if n > 0
      push_ba.each{|pushs|
        pushs.each{|r|
          commit "  PUSH(SCREG(#{r}));"
        }
      }
    end

    def make_header_operands insn
      comment "  /* declare and get from iseq */"

      vars = insn.opes
      n = 0
      ops = []

      vars.each_with_index{|(type, var), i|
        if type == '...'
          break
        end

        re = /\b#{var}\b/n
        if re =~ insn.body or re =~ insn.sp_inc or insn.rets.any?{|t, v| re =~ v} or re =~ 'ic'
          ops << "  #{type} #{var} = (#{type})GET_OPERAND(#{i+1});"
        end
        n   += 1
      }
      @opn = n

      # reverse or not?
      # ops.join
      commit ops.reverse
    end

    def make_header_default_operands insn
      vars = insn.defopes

      vars.each{|e|
        next if e[1] == '*'
        if use_const?
          commit "  const #{e[0][0]} #{e[0][1]} = #{e[1]};"
        else
          commit "  #define #{e[0][1]} #{e[1]}"
        end
      }
    end

    def make_footer_default_operands insn
      comment " /* declare and initialize default opes */"
      if use_const?
        commit
      else
        vars = insn.defopes

        vars.each{|e|
          next if e[1] == '*'
          commit "#undef #{e[0][1]}"
        }
      end
    end

    def make_header_stack_pops insn
      comment "  /* declare and pop from stack */"

      n = 0
      pops = []
      vars = insn.pops
      vars.each_with_index{|iter, i|
        type, var, r = *iter
        if type == '...'
          break
        end
        if r
          pops << "  #{type} #{var} = SCREG(#{r});"
        else
          pops << "  #{type} #{var} = TOPN(#{n});"
          n += 1
        end
      }
      @popn = n

      # reverse or not?
      commit pops.reverse
    end

    def make_header_temporary_vars insn
      comment "  /* declare temporary vars */"

      insn.tvars.each{|var|
        commit "  #{var[0]} #{var[1]};"
      }
    end

    def make_header_stack_val insn
      comment "/* declare stack push val */"

      vars = insn.opes + insn.pops + insn.defopes.map{|e| e[0]}

      insn.rets.each{|var|
        if vars.all?{|e| e[1] != var[1]} && var[1] != '...'
          commit "  #{var[0]} #{var[1]};"
        end
      }
    end

    def make_header_analysis insn
      commit "  USAGE_ANALYSIS_INSN(BIN(#{insn.name}));"
      insn.opes.each_with_index{|op, i|
        commit "  USAGE_ANALYSIS_OPERAND(BIN(#{insn.name}), #{i}, #{op[1]});"
      }
    end

    def make_header_pc insn
      commit  "  ADD_PC(1+#{@opn});"
      commit  "  PREFETCH(GET_PC());"
    end

    def make_header_popn insn
      comment "  /* management */"
      commit  "  POPN(#{@popn});" if @popn > 0
    end

    def make_hader_debug insn
      comment "  /* for debug */"
      commit  "  DEBUG_ENTER_INSN(\"#{insn.name}\");"
    end

    def make_header_defines insn
      commit  "  #define CURRENT_INSN_#{insn.name} 1"
      commit  "  #define INSN_IS_SC()     #{insn.sc ? 0 : 1}"
      commit  "  #define INSN_LABEL(lab)  LABEL_#{insn.name}_##lab"
      commit  "  #define LABEL_IS_SC(lab) LABEL_##lab##_###{insn.sc.size == 0 ? 't' : 'f'}"
    end

    def each_footer_stack_val insn
      insn.rets.reverse_each{|v|
        break if v[1] == '...'
        yield v
      }
    end

    def make_footer_stack_val insn
      comment "  /* push stack val */"

      n = 0
      each_footer_stack_val(insn){|v|
        n += 1 unless v[2]
      }
      commit "  CHECK_STACK_OVERFLOW(REG_CFP, #{n});" if n > 0
      each_footer_stack_val(insn){|v|
        if v[2]
          commit "  SCREG(#{v[2]}) = #{v[1]};"
        else
          commit "  PUSH(#{v[1]});"
        end
      }
    end

    def make_footer_undefs insn
      commit "#undef CURRENT_INSN_#{insn.name}"
      commit "#undef INSN_IS_SC"
      commit "#undef INSN_LABEL"
      commit "#undef LABEL_IS_SC"
    end

    def make_header insn
      commit "INSN_ENTRY(#{insn.name}){"
      make_header_prepare_stack insn
      commit "{"
      make_header_stack_val  insn
      make_header_default_operands insn
      make_header_operands   insn
      make_header_stack_pops insn
      make_header_temporary_vars insn
      #
      make_hader_debug insn
      make_header_pc insn
      make_header_popn insn
      make_header_defines insn
      make_header_analysis insn
      commit  "{"
    end

    def make_footer insn
      make_footer_stack_val insn
      make_footer_default_operands insn
      make_footer_undefs insn
      commit "  END_INSN(#{insn.name});}}}"
    end

    def make_insn_def insn
      build_string do
        make_header insn
        if line = insn.body.instance_variable_get(:@line_no)
          file = insn.body.instance_variable_get(:@file)
          commit "#line #{line+1} \"#{file}\""
          commit insn.body
          commit '#line __CURRENT_LINE__ "__CURRENT_FILE__"'
        else
          insn.body
        end
        make_footer(insn)
      end
    end
  end

  ###################################################################
  # vmtc.inc
  class VmTCIncGenerator < SourceCodeGenerator
    def generate

      insns_table = build_string do
        @insns.each{|insn|
          commit "  LABEL_PTR(#{insn.name}),"
        }
      end

      insn_end_table = build_string do
        @insns.each{|insn|
          commit "  ELABEL_PTR(#{insn.name}),\n"
        }
      end

      ERB.new(vpath.read('template/vmtc.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # insns_info.inc
  class InsnsInfoIncGenerator < SourceCodeGenerator
    def generate
      insns_info_inc
    end

    ###
    private

    def op2typesig op
      case op
      when /^OFFSET/
        "TS_OFFSET"
      when /^rb_num_t/
        "TS_NUM"
      when /^lindex_t/
        "TS_LINDEX"
      when /^dindex_t/
        "TS_DINDEX"
      when /^VALUE/
        "TS_VALUE"
      when /^ID/
        "TS_ID"
      when /GENTRY/
        "TS_GENTRY"
      when /^IC/
        "TS_IC"
      when /^\.\.\./
        "TS_VARIABLE"
      when /^CDHASH/
        "TS_CDHASH"
      when /^ISEQ/
        "TS_ISEQ"
      when /rb_insn_func_t/
        "TS_FUNCPTR"
      else
        raise "unknown op type: #{op}"
      end
    end

    TYPE_CHARS = {
      'TS_OFFSET'    => 'O',
      'TS_NUM'       => 'N',
      'TS_LINDEX'    => 'L',
      'TS_DINDEX'    => 'D',
      'TS_VALUE'     => 'V',
      'TS_ID'        => 'I',
      'TS_GENTRY'    => 'G',
      'TS_IC'        => 'C',
      'TS_CDHASH'    => 'H',
      'TS_ISEQ'      => 'S',
      'TS_VARIABLE'  => '.',
      'TS_FUNCPTR'   => 'F',
    }

    # insns_info.inc
    def insns_info_inc
      # insn_type_chars
      insn_type_chars = TYPE_CHARS.map{|t, c|
        "#define #{t} '#{c}'"
      }.join("\n")

      # insn_names
      insn_names = ''
      @insns.each{|insn|
        insn_names << "  \"#{insn.name}\",\n"
      }

      # operands info
      operands_info = ''
      operands_num_info = ''

      @insns.each{|insn|
        opes = insn.opes
        operands_info << '  '
        ot = opes.map{|type, var|
          TYPE_CHARS.fetch(op2typesig(type))
        }
        operands_info << "\"#{ot.join}\"" << ", \n"

        num = opes.size + 1
        operands_num_info << "  #{num},\n"
      }

      # stack num
      stack_num_info = ''
      @insns.each{|insn|
        num = insn.rets.size
        stack_num_info << "  #{num},\n"
      }

      # stack increase
      stack_increase = ''
      @insns.each{|insn|
        stack_increase << <<-EOS
        case BIN(#{insn.name}):{
          #{insn.sp_increase_c_expr}
        }
        EOS
      }
      ERB.new(vpath.read('template/insns_info.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # insns.inc
  class InsnsIncGenerator < SourceCodeGenerator
    def generate
      i=0
      insns = build_string do
        @insns.each{|insn|
          commit "  %-30s = %d,\n" % ["BIN(#{insn.name})", i]
          i+=1
        }
      end

      ERB.new(vpath.read('template/insns.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # minsns.inc
  class MInsnsIncGenerator < SourceCodeGenerator
    def generate
      i=0
      defs = build_string do
        @insns.each{|insn|
          commit "  rb_define_const(mYarvInsns, %-30s, INT2FIX(%d));\n" %
                 ["\"I#{insn.name}\"", i]
          i+=1
        }
      end
      ERB.new(vpath.read('template/minsns.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # optinsn.inc
  class OptInsnIncGenerator < SourceCodeGenerator
    def generate
      optinsn_inc
    end

    ###
    private

    def val_as_type op
      type = op[0][0]
      val  = op[1]

      case type
      when /^long/, /^rb_num_t/, /^lindex_t/, /^dindex_t/
        "INT2FIX(#{val})"
      when /^VALUE/
        val
      when /^ID/
        "INT2FIX(#{val})"
      when /^ISEQ/, /^rb_insn_func_t/
        val
      when /GENTRY/
        raise
      when /^\.\.\./
        raise
      else
        raise "type: #{type}"
      end
    end

    # optinsn.inc
    def optinsn_inc
      rule = ''
      opt_insns_map = Hash.new{|h, k| h[k] = []}

      @insns.each{|insn|
        next if insn.defopes.size == 0
        next if insn.type         == :sc
        next if /^UNIFIED/ =~ insn.name.to_s

        originsn = insn.orig
        opt_insns_map[originsn] << insn
      }

      rule = build_string do
        opt_insns_map.each{|originsn, optinsns|
          commit "case BIN(#{originsn.name}):"

          optinsns.sort_by{|opti|
            opti.defopes.find_all{|e| e[1] == '*'}.size
          }.each{|opti|
            commit "  if("
            i = 0
            commit "    " + opti.defopes.map{|opinfo|
              i += 1
              next if opinfo[1] == '*'
              "insnobj->operands[#{i-1}] == #{val_as_type(opinfo)}"
            }.compact.join('&&  ')
            commit "  ){"
            idx = 0
            n = 0
            opti.defopes.each{|opinfo|
              if opinfo[1] == '*'
                if idx != n
                  commit "    insnobj->operands[#{idx}] = insnobj->operands[#{n}];"
                end
                idx += 1
              else
                # skip
              end
              n += 1
            }
            commit "    insnobj->insn_id = BIN(#{opti.name});"
            commit "    insnobj->operand_size = #{idx};"
            commit "    break;\n  }\n"
          }
          commit "  break;";
        }
      end

      ERB.new(vpath.read('template/optinsn.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # optunifs.inc
  class OptUnifsIncGenerator < SourceCodeGenerator
    def generate
      unif_insns_each = ''
      unif_insns      = ''
      unif_insns_data = []

      insns = @insns.find_all{|insn| !insn.is_sc}
      insns.each{|insn|
        size = insn.unifs.size
        if size > 0
          insn.unifs.sort_by{|unif| -unif[1].size}.each_with_index{|unif, i|

          uni_insn, uni_insns = *unif
          uni_insns = uni_insns[1..-1]
          unif_insns_each << "static const int UNIFIED_#{insn.name}_#{i}[] = {" +
                             "  BIN(#{uni_insn.name}), #{uni_insns.size + 2}, \n  " +
                             uni_insns.map{|e| "BIN(#{e.name})"}.join(", ") + "};\n"
          }
        else

        end
        if size > 0
          unif_insns << "static const int *const UNIFIED_#{insn.name}[] = {(int *)#{size+1}, \n"
          unif_insns << (0...size).map{|e| "  UNIFIED_#{insn.name}_#{e}"}.join(",\n") + "};\n"
          unif_insns_data << "  UNIFIED_#{insn.name}"
        else
          unif_insns_data << "  0"
        end
      }
      unif_insns_data = "static const int *const *const unified_insns_data[] = {\n" +
                        unif_insns_data.join(",\n") + "};\n"
      ERB.new(vpath.read('template/optunifs.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # opt_sc.inc
  class OptSCIncGenerator < SourceCodeGenerator
    def generate
      sc_insn_info = []
      @insns.each{|insn|
        insns = insn.sc
        if insns.size > 0
          insns = ['SC_ERROR'] + insns.map{|e| "    BIN(#{e.name})"}
        else
          insns = Array.new(6){'SC_ERROR'}
        end
        sc_insn_info << "  {\n#{insns.join(",\n")}}"
      }
      sc_insn_info = sc_insn_info.join(",\n")

      sc_insn_next = @insns.map{|insn|
        "  SCS_#{InstructionsLoader.complement_name(insn.nextsc).upcase}" +
        (verbose? ? " /* #{insn.name} */" : '')
      }.join(",\n")
      ERB.new(vpath.read('template/opt_sc.inc.tmpl')).result(binding)
    end
  end

  ###################################################################
  # yasmdata.rb
  class YASMDataRbGenerator < SourceCodeGenerator
    def generate
      insn_id2no = ''
      @insns.each_with_index{|insn, i|
        insn_id2no << "        :#{insn.name} => #{i},\n"
      }
      ERB.new(vpath.read('template/yasmdata.rb.tmpl')).result(binding)
    end
  end

  ###################################################################
  # yarvarch.*
  class YARVDocGenerator < SourceCodeGenerator
    def generate

    end

    def desc lang
      d = ''
      i = 0
      cat = nil
      @insns.each{|insn|
        seq    = insn.opes.map{|t,v| v}.join(' ')
        before = insn.pops.reverse.map{|t,v| v}.join(' ')
        after  = insn.rets.reverse.map{|t,v| v}.join(' ')

        if cat != insn.comm[:c]
          d << "** #{insn.comm[:c]}\n\n"
          cat = insn.comm[:c]
        end

        d << "*** #{insn.name}\n"
        d << "\n"
        d << insn.comm[lang] + "\n\n"
        d << ":instruction sequence: 0x%02x #{seq}\n" % i
        d << ":stack: #{before} => #{after}\n\n"
        i+=1
      }
      d
    end

    def desc_ja
      d = desc :j
      ERB.new(vpath.read('template/yarvarch.ja')).result(binding)
    end

    def desc_en
      d = desc :e
      ERB.new(vpath.read('template/yarvarch.en')).result(binding)
    end
  end

  module VPATH
    def search(meth, base, *rest)
      begin
        meth.call(base, *rest)
      rescue Errno::ENOENT => error
        each do |dir|
          return meth.call(File.join(dir, base), *rest) rescue nil
        end
        raise error
      end
    end

    def process(*args, &block)
      search(File.method(__callee__), *args, &block)
    end

    alias stat process
    alias lstat process

    def open(*args)
      f = search(File.method(:open), *args)
      if block_given?
        begin
          yield f
        ensure
          f.close unless f.closed?
        end
      else
        f
      end
    end

    def read(*args)
      open(*args) {|f| f.read}
    end

    def foreach(file, *args, &block)
      open(file) {|f| f.each(*args, &block)}
    end

    def self.def_options(opt)
      vpath = []
      path_sep = ':'

      opt.on("-I", "--srcdir=DIR", "add a directory to search path") {|dir|
        vpath |= [dir]
      }
      opt.on("-L", "--vpath=PATH LIST", "add directories to search path") {|dirs|
        vpath |= dirs.split(path_sep)
      }
      opt.on("--path-separator=SEP", /\A\W\z/, "separator for vpath") {|sep|
        path_sep = sep
      }

      proc {
        vpath.extend(self) unless vpath.empty?
      }
    end
  end

  class SourceCodeGenerator
    Files = { # codes
      'vm.inc'         => VmBodyGenerator,
      'vmtc.inc'       => VmTCIncGenerator,
      'insns.inc'      => InsnsIncGenerator,
      'insns_info.inc' => InsnsInfoIncGenerator,
    # 'minsns.inc'     => MInsnsIncGenerator,
      'optinsn.inc'    => OptInsnIncGenerator,
      'optunifs.inc'   => OptUnifsIncGenerator,
      'opt_sc.inc'     => OptSCIncGenerator,
      'yasmdata.rb'    => YASMDataRbGenerator,
    }

    def generate args = []
      args = Files.keys if args.empty?
      args.each{|fn|
        s = Files[fn].new(@insns).generate
        open(output_path(fn), 'w') {|f| f.puts(s)}
      }
    end

    def self.def_options(opt)
      opts = {
        :"insns.def" => 'insns.def',
        :"opope.def" => 'defs/opt_operand.def',
        :"unif.def"  => 'defs/opt_insn_unif.def',
      }

      opt.on("-Dname", /\AOPT_(\w+)\z/, "enable VM option") {|s, v|
        opts[v] = true
      }
      opt.on("--enable=name[,name...]", Array,
        "enable VM options (without OPT_ prefix)") {|*a|
        a.each {|v| opts[v] = true}
      }
      opt.on("-Uname", /\AOPT_(\w+)\z/, "disable VM option") {|s, v|
        opts[v] = false
      }
      opt.on("--disable=name[,name...]", Array,
        "disable VM options (without OPT_ prefix)") {|*a|
          a.each {|v| opts[v] = false}
      }
      opt.on("-i", "--insnsdef=FILE", "--instructions-def",
        "instructions definition file") {|n|
        opts[:insns_def] = n
      }
      opt.on("-o", "--opt-operanddef=FILE", "--opt-operand-def",
        "vm option: operand definition file") {|n|
        opts[:opope_def] = n
      }
      opt.on("-u", "--opt-insnunifdef=FILE", "--opt-insn-unif-def",
        "vm option: instruction unification file") {|n|
        opts[:unif_def] = n
      }
      opt.on("-C", "--[no-]use-const",
        "use consts for default operands instead of macros") {|v|
        opts[:use_const] = v
      }
      opt.on("-d", "--destdir", "--output-directory=DIR",
        "make output file underneath DIR") {|v|
        opts[:destdir] = v
      }
      opt.on("-V", "--[no-]verbose") {|v|
        opts[:verbose] = v
      }

      vpath = VPATH.def_options(opt)

      proc {
        opts[:VPATH] = vpath.call
        build opts
      }
    end

    def self.build opts, vpath = ['./']
      opts[:VPATH] = vpath.extend(VPATH) unless opts[:VPATH]
      self.new InstructionsLoader.new(opts)
    end
  end
end

