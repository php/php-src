# prepare 'fasta.output'

def prepare_fasta_output n
  filebase = File.join(File.dirname($0), 'fasta.output')
  script = File.join(File.dirname($0), 'bm_so_fasta.rb')
  file = "#{filebase}.#{n}"

  unless FileTest.exist?(file)
    STDERR.puts "preparing #{file}"

    open(file, 'w'){|f|
      ARGV[0] = n
      $stdout = f
      load script
      $stdout = STDOUT
    }
  end
end

