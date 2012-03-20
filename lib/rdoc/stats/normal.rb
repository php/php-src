##
# Stats printer that prints just the files being documented with a progress
# bar

class RDoc::Stats::Normal < RDoc::Stats::Quiet

  def begin_adding # :nodoc:
    puts "Parsing sources..." if $stdout.tty?
  end

  ##
  # Prints a file with a progress bar

  def print_file files_so_far, filename
    return unless $stdout.tty?

    progress_bar = sprintf("%3d%% [%2d/%2d]  ",
                           100 * files_so_far / @num_files,
                           files_so_far,
                           @num_files)

    # Print a progress bar, but make sure it fits on a single line. Filename
    # will be truncated if necessary.
    terminal_width = (ENV['COLUMNS'] || 80).to_i
    max_filename_size = terminal_width - progress_bar.size

    if filename.size > max_filename_size then
      # Turn "some_long_filename.rb" to "...ong_filename.rb"
      filename = filename[(filename.size - max_filename_size) .. -1]
      filename[0..2] = "..."
    end

    # Pad the line with whitespaces so that leftover output from the
    # previous line doesn't show up.
    line = "#{progress_bar}#{filename}"
    padding = terminal_width - line.size
    line << (" " * padding) if padding > 0

    $stdout.print("#{line}\r")
    $stdout.flush
  end

  def done_adding # :nodoc:
    puts if $stdout.tty?
  end

end

