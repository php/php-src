#!/usr/bin/env ruby
#
# The Computer Language Shootout
#   http://shootout.alioth.debian.org
#   contributed by Kevin Barnes (Ruby novice)

# PROGRAM:  the main body is at the bottom.
#   1) read about the problem here: http://www-128.ibm.com/developerworks/java/library/j-javaopt/
#   2) see how I represent a board as a bitmask by reading the blank_board comments
#   3) read as your mental paths take you

def print *args
end

# class to represent all information about a particular rotation of a particular piece
class Rotation
  # an array (by location) containing a bit mask for how the piece maps at the given location.
  # if the rotation is invalid at that location the mask will contain false
  attr_reader :start_masks

  # maps a direction to a relative location.  these differ depending on whether it is an even or
  # odd row being mapped from
  @@rotation_even_adder = { :west => -1, :east => 1, :nw => -7, :ne => -6, :sw => 5, :se => 6 }
  @@rotation_odd_adder = { :west => -1, :east => 1, :nw => -6, :ne => -5, :sw => 6, :se => 7 }

  def initialize( directions )
    @even_offsets, @odd_offsets = normalize_offsets( get_values( directions ))

    @even_mask = mask_for_offsets( @even_offsets)
    @odd_mask = mask_for_offsets( @odd_offsets)

    @start_masks = Array.new(60)

    # create the rotational masks by placing the base mask at the location and seeing if
    # 1) it overlaps the boundries and 2) it produces a prunable board.  if either of these
    # is true the piece cannot be placed
    0.upto(59) do | offset |
      mask = is_even(offset) ? (@even_mask << offset) : (@odd_mask << offset)
      if (blank_board & mask == 0 && !prunable(blank_board | mask, 0, true)) then
        imask = compute_required( mask, offset)
        @start_masks[offset] = [ mask, imask, imask | mask ]
      else
        @start_masks[offset] = false
      end
    end
  end

  def compute_required( mask, offset )
    board = blank_board
    0.upto(offset) { | i | board |= 1 << i }
    board |= mask
    return 0 if (!prunable(board | mask, offset))
    board = flood_fill(board,58)
    count = 0
    imask = 0
    0.upto(59) do | i |
      if (board[i] == 0) then
        imask |= (1 << i)
        count += 1
      end
    end
    (count > 0 && count < 5) ? imask : 0
  end

  def flood_fill( board, location)
    return board if (board[location] == 1)
    board |= 1 << location
    row, col = location.divmod(6)
    board = flood_fill( board, location - 1) if (col > 0)
    board = flood_fill( board, location + 1) if (col < 4)
    if (row % 2 == 0) then
      board = flood_fill( board, location - 7) if (col > 0 && row > 0)
      board = flood_fill( board, location - 6) if (row > 0)
      board = flood_fill( board, location + 6) if (row < 9)
      board = flood_fill( board, location + 5) if (col > 0 && row < 9)
    else
      board = flood_fill( board, location - 5) if (col < 4 && row > 0)
      board = flood_fill( board, location - 6) if (row > 0)
      board = flood_fill( board, location + 6) if (row < 9)
      board = flood_fill( board, location + 7) if (col < 4 && row < 9)
    end
    board
  end

  # given a location, produces a list of relative locations covered by the piece at this rotation
  def offsets( location)
    if is_even( location) then
      @even_offsets.collect { | value | value + location }
    else
      @odd_offsets.collect { | value | value + location }
    end
  end

  # returns a set of offsets relative to the top-left most piece of the rotation (by even or odd rows)
  # this is hard to explain. imagine we have this partial board:
  #   0 0 0 0 0 x        [positions 0-5]
  #    0 0 1 1 0 x       [positions 6-11]
  #   0 0 1 0 0 x        [positions 12-17]
  #    0 1 0 0 0 x       [positions 18-23]
  #   0 1 0 0 0 x        [positions 24-29]
  #    0 0 0 0 0 x       [positions 30-35]
  #       ...
  # The top-left of the piece is at position 8, the
  # board would be passed as a set of positions (values array) containing [8,9,14,19,25] not necessarily in that
  # sorted order.  Since that array starts on an odd row, the offsets for an odd row are: [0,1,6,11,17] obtained
  # by subtracting 8 from everything.  Now imagine the piece shifted up and to the right so it's on an even row:
  #   0 0 0 1 1 x        [positions 0-5]
  #    0 0 1 0 0 x       [positions 6-11]
  #   0 0 1 0 0 x        [positions 12-17]
  #    0 1 0 0 0 x       [positions 18-23]
  #   0 0 0 0 0 x        [positions 24-29]
  #    0 0 0 0 0 x       [positions 30-35]
  #       ...
  # Now the positions are [3,4,8,14,19] which after subtracting the lowest value (3) gives [0,1,5,11,16] thus, the
  # offsets for this particular piece are (in even, odd order) [0,1,5,11,16],[0,1,6,11,17] which is what
  # this function would return
  def normalize_offsets( values)
    min = values.min
    even_min = is_even(min)
    other_min = even_min ? min + 6 : min + 7
    other_values = values.collect do | value |
      if is_even(value) then
        value + 6 - other_min
      else
        value + 7 - other_min
      end
    end
    values.collect! { | value | value - min }

    if even_min then
      [values, other_values]
    else
      [other_values, values]
    end
  end

  # produce a bitmask representation of an array of offset locations
  def mask_for_offsets( offsets )
    mask = 0
    offsets.each { | value | mask = mask + ( 1 << value ) }
    mask
  end

  # finds a "safe" position that a position as described by a list of directions can be placed
  # without falling off any edge of the board.  the values returned a location to place the first piece
  # at so it will fit after making the described moves
  def start_adjust( directions )
    south = east = 0;
    directions.each do | direction |
      east += 1 if ( direction == :sw || direction == :nw || direction == :west )
      south += 1 if ( direction == :nw || direction == :ne )
    end
    south * 6 + east
  end

  # given a set of directions places the piece (as defined by a set of directions) on the board at
  # a location that will not take it off the edge
  def get_values ( directions )
    start = start_adjust(directions)
    values = [ start ]
    directions.each do | direction |
      if (start % 12 >= 6) then
        start += @@rotation_odd_adder[direction]
      else
        start += @@rotation_even_adder[direction]
      end
      values += [ start ]
    end

    # some moves take you back to an existing location, we'll strip duplicates
    values.uniq
  end
end

# describes a piece and caches information about its rotations to as to be efficient for iteration
# ATTRIBUTES:
#   rotations -- all the rotations of the piece
#   type -- a numeic "name" of the piece
#   masks -- an array by location of all legal rotational masks (a n inner array) for that location
#   placed -- the mask that this piece was last placed at (not a location, but the actual mask used)
class Piece
  attr_reader :rotations, :type, :masks
  attr_accessor :placed

  # transform hashes that change one direction into another when you either flip or rotate a set of directions
  @@flip_converter = { :west => :west, :east => :east, :nw => :sw, :ne => :se, :sw => :nw, :se => :ne }
  @@rotate_converter = { :west => :nw, :east => :se, :nw => :ne, :ne => :east, :sw => :west, :se => :sw }

  def initialize( directions, type )
    @type = type
    @rotations = Array.new();
    @map = {}

    generate_rotations( directions )
    directions.collect! { | value | @@flip_converter[value] }
    generate_rotations( directions )

    # creates the masks AND a map that returns [location, rotation] for any given mask
    # this is used when a board is found and we want to draw it, otherwise the map is unused
    @masks = Array.new();
    0.upto(59) do | i |
      even = true
      @masks[i] = @rotations.collect do | rotation |
        mask = rotation.start_masks[i]
        @map[mask[0]] = [ i, rotation ] if (mask)
        mask || nil
      end
      @masks[i].compact!
    end
  end

  # rotates a set of directions through all six angles and adds a Rotation to the list for each one
  def generate_rotations( directions )
    6.times do
      rotations.push( Rotation.new(directions))
      directions.collect! { | value | @@rotate_converter[value] }
    end
  end

  # given a board string, adds this piece to the board at whatever location/rotation
  # important: the outbound board string is 5 wide, the normal location notation is six wide (padded)
  def fill_string( board_string)
    location, rotation = @map[@placed]
    rotation.offsets(location).each do | offset |
      row, col = offset.divmod(6)
      board_string[ row*5 + col, 1 ] = @type.to_s
    end
  end
end

# a blank bit board having this form:
#
#    0 0 0 0 0 1
#     0 0 0 0 0 1
#    0 0 0 0 0 1
#     0 0 0 0 0 1
#    0 0 0 0 0 1
#     0 0 0 0 0 1
#    0 0 0 0 0 1
#     0 0 0 0 0 1
#    0 0 0 0 0 1
#     0 0 0 0 0 1
#    1 1 1 1 1 1
#
# where left lest significant bit is the top left and the most significant is the lower right
# the actual board only consists of the 0 places, the 1 places are blockers to keep things from running
# off the edges or bottom
def blank_board
  0b111111100000100000100000100000100000100000100000100000100000100000
end

def full_board
  0b111111111111111111111111111111111111111111111111111111111111111111
end

# determines if a location (bit position) is in an even row
def is_even( location)
  (location % 12) < 6
end

# support function that create three utility maps:
#  $converter -- for each row an array that maps a five bit row (via array mapping)
#                to the a a five bit representation of the bits below it
#  $bit_count -- maps a five bit row (via array mapping) to the number of 1s in the row
#  @@new_regions -- maps a five bit row (via array mapping) to an array of "region" arrays
#                   a region array has three values the first is a mask of bits in the region,
#                   the second is the count of those bits and the third is identical to the first
#                   examples:
#                           0b10010 => [ 0b01100, 2, 0b01100 ], [ 0b00001, 1, 0b00001]
#                           0b01010 => [ 0b10000, 1, 0b10000 ], [ 0b00100, 1, 0b00100 ], [ 0b00001, 1, 0b00001]
#                           0b10001 => [ 0b01110, 3, 0b01110 ]
def create_collector_support
  odd_map = [0b11, 0b110, 0b1100, 0b11000, 0b10000]
  even_map = [0b1, 0b11, 0b110, 0b1100, 0b11000]

  all_odds = Array.new(0b100000)
  all_evens = Array.new(0b100000)
  bit_counts = Array.new(0b100000)
  new_regions = Array.new(0b100000)
  0.upto(0b11111) do | i |
    bit_count = odd = even = 0
    0.upto(4) do | bit |
      if (i[bit] == 1) then
        bit_count += 1
        odd |= odd_map[bit]
        even |= even_map[bit]
      end
    end
    all_odds[i] = odd
    all_evens[i] = even
    bit_counts[i] = bit_count
    new_regions[i] = create_regions( i)
  end

  $converter = []
  10.times { | row | $converter.push((row % 2 == 0) ? all_evens : all_odds) }
  $bit_counts = bit_counts
  $regions = new_regions.collect { | set | set.collect { | value | [ value, bit_counts[value], value] } }
end

# determines if a board is punable, meaning that there is no possibility that it
# can be filled up with pieces.  A board is prunable if there is a grouping of unfilled spaces
# that are not a multiple of five.  The following board is an example of a prunable board:
#    0 0 1 0 0
#     0 1 0 0 0
#    1 1 0 0 0
#     0 1 0 0 0
#    0 0 0 0 0
#       ...
#
# This board is prunable because the top left corner is only 3 bits in area, no piece will ever fit it
# parameters:
#   board -- an initial bit board (6 bit padded rows, see blank_board for format)
#   location -- starting location, everything above and to the left is already full
#   slotting -- set to true only when testing initial pieces, when filling normally
#               additional assumptions are possible
#
# Algorithm:
#    The algorithm starts at the top row (as determined by location) and iterates a row at a time
#    maintainng counts of active open areas (kept in the collector array) each collector contains
#    three values at the start of an iteration:
#          0: mask of bits that would be adjacent to the collector in this row
#          1: the number of bits collected so far
#          2: a scratch space starting as zero, but used during the computation to represent
#             the empty bits in the new row that are adjacent (position 0)
#  The exact procedure is described in-code
def prunable( board, location, slotting = false)
  collectors = []
  # loop accross the rows
  (location / 6).to_i.upto(9) do | row_on |
    # obtain a set of regions representing the bits of the curent row.
    regions = $regions[(board >> (row_on * 6)) & 0b11111]
    converter = $converter[row_on]

    # track the number of collectors at the start of the cycle so that
    # we don't compute against newly created collectors, only existing collectors
    initial_collector_count = collectors.length

    # loop against the regions.  For each region of the row
    # we will see if it connects to one or more existing collectors.
    # if it connects to 1 collector, the bits from the region are added to the
    # bits of the collector and the mask is placed in collector[2]
    # If the region overlaps more than one collector then all the collectors
    # it overlaps with are merged into the first one (the others are set to nil in the array)
    # if NO collectors are found then the region is copied as a new collector
    regions.each do | region |
      collector_found = nil
      region_mask = region[2]
      initial_collector_count.times do | collector_num |
        collector = collectors[collector_num]
        if (collector) then
          collector_mask = collector[0]
          if (collector_mask & region_mask != 0) then
            if (collector_found) then
              collector_found[0] |= collector_mask
              collector_found[1] += collector[1]
              collector_found[2] |= collector[2]
              collectors[collector_num] = nil
            else
              collector_found = collector
              collector[1] += region[1]
              collector[2] |= region_mask
            end
          end
        end
      end
      if (collector_found == nil) then
        collectors.push(Array.new(region))
      end
    end

    # check the existing collectors, if any collector overlapped no bits in the region its [2] value will
    # be zero.  The size of any such reaason is tested if it is not a muliple of five true is returned since
    # the board is prunable.  if it is a multiple of five it is removed.
    # Collector that are still active have a new adjacent value [0] set based n the matched bits
    # and have [2] cleared out for the next cycle.
    collectors.length.times do | collector_num |
      collector = collectors[collector_num]
      if (collector) then
        if (collector[2] == 0) then
          return true if (collector[1] % 5 != 0)
          collectors[collector_num] = nil
        else
          # if a collector matches all bits in the row then we can return unprunable early for the
          # follwing reasons:
          #    1) there can be no more unavailable bits bince we fill from the top left downward
          #    2) all previous regions have been closed or joined so only this region can fail
          #    3) this region must be good since there can never be only 1 region that is nuot
          #       a multiple of five
          # this rule only applies when filling normally, so we ignore the rule if we are "slotting"
          # in pieces to see what configurations work for them (the only other time this algorithm is used).
          return false if (collector[2] == 0b11111 && !slotting)
          collector[0] = converter[collector[2]]
          collector[2] = 0
        end
      end
    end

    # get rid of all the empty converters for the next round
    collectors.compact!
  end
  return false if (collectors.length <= 1) # 1 collector or less and the region is fine
  collectors.any? { | collector | (collector[1] % 5) != 0 } # more than 1 and we test them all for bad size
end

# creates a region given a row mask.  see prunable for what a "region" is
def create_regions( value )
  regions = []
  cur_region = 0
  5.times do | bit |
    if (value[bit] == 0) then
      cur_region |= 1 << bit
    else
      if (cur_region != 0 ) then
        regions.push( cur_region)
        cur_region = 0;
      end
    end
  end
  regions.push(cur_region) if (cur_region != 0)
  regions
end

# find up to the counted number of solutions (or all solutions) and prints the final result
def find_all
  find_top( 1)
  find_top( 0)
  print_results
end

# show the board
def print_results
  print "#{@boards_found} solutions found\n\n"
  print_full_board( @min_board)
  print "\n"
  print_full_board( @max_board)
  print "\n"
end

# finds solutions.  This special version of the main function is only used for the top level
# the reason for it is basically to force a particular ordering on how the rotations are tested for
# the first piece.  It is called twice, first looking for placements of the odd rotations and then
# looking for placements of the even locations.
#
# WHY?
#   Since any found solution has an inverse we want to maximize finding solutions that are not already found
#   as an inverse.  The inverse will ALWAYS be 3 one of the piece configurations that is exactly 3 rotations away
#   (an odd number).  Checking even vs odd then produces a higher probability of finding more pieces earlier
#   in the cycle.  We still need to keep checking all the permutations, but our probability of finding one will
#   diminsh over time.  Since we are TOLD how many to search for this lets us exit before checking all pieces
#   this bennifit is very great when seeking small numbers of solutions and is 0 when looking for more than the
#   maximum number
def find_top( rotation_skip)
  board = blank_board
  (@pieces.length-1).times do
    piece = @pieces.shift
    piece.masks[0].each do | mask, imask, cmask |
      if ((rotation_skip += 1) % 2 == 0) then
        piece.placed = mask
        find( 1, 1, board | mask)
      end
    end
    @pieces.push(piece)
  end
  piece = @pieces.shift
  @pieces.push(piece)
end

# the normail find routine, iterates through the available pieces, checks all rotations at the current location
# and adds any boards found.  depth is acheived via recursion.  the overall approach is described
# here: http://www-128.ibm.com/developerworks/java/library/j-javaopt/
# parameters:
#  start_location -- where to start looking for place for the next piece at
#  placed -- number of pieces placed
#  board -- current state of the board
#
# see in-code comments
def find( start_location, placed, board)
  # find the next location to place a piece by looking for an empty bit
  while board[start_location] == 1
    start_location += 1
  end

  @pieces.length.times do
    piece = @pieces.shift
    piece.masks[start_location].each do | mask, imask, cmask |
      if ( board & cmask == imask) then
        piece.placed = mask
        if (placed == 9) then
          add_board
        else
          find( start_location + 1, placed + 1, board | mask)
        end
      end
    end
    @pieces.push(piece)
  end
end

# print the board
def print_full_board( board_string)
  10.times do | row |
    print " " if (row % 2 == 1)
    5.times do | col |
      print "#{board_string[row*5 + col,1]} "
    end
    print "\n"
  end
end

# when a board is found we "draw it" into a string and then flip that string, adding both to
# the list (hash) of solutions if they are unique.
def add_board
  board_string = "99999999999999999999999999999999999999999999999999"
  @all_pieces.each {  | piece | piece.fill_string( board_string ) }
  save( board_string)
  save( board_string.reverse)
end

# adds a board string to the list (if new) and updates the current best/worst board
def save( board_string)
  if (@all_boards[board_string] == nil) then
    @min_board = board_string if (board_string < @min_board)
    @max_board = board_string if (board_string > @max_board)
    @all_boards.store(board_string,true)
    @boards_found += 1

    # the exit motif is a time saver.  Ideally the function should return, but those tests
    # take noticable time (performance).
    if (@boards_found == @stop_count) then
      print_results
      exit(0)
    end
  end
end


##
## MAIN BODY :)
##
create_collector_support
@pieces = [
  Piece.new( [ :nw, :ne, :east, :east ], 2),
  Piece.new( [ :ne, :se, :east, :ne ], 7),
  Piece.new( [ :ne, :east, :ne, :nw ], 1),
  Piece.new( [ :east, :sw, :sw, :se ], 6),
  Piece.new( [ :east, :ne, :se, :ne ], 5),
  Piece.new( [ :east, :east, :east, :se ], 0),
  Piece.new( [ :ne, :nw, :se, :east, :se ], 4),
  Piece.new( [ :se, :se, :se, :west ], 9),
  Piece.new( [ :se, :se, :east, :se ], 8),
  Piece.new( [ :east, :east, :sw, :se ], 3)
  ];

@all_pieces = Array.new( @pieces)

@min_board = "99999999999999999999999999999999999999999999999999"
@max_board = "00000000000000000000000000000000000000000000000000"
@stop_count = ARGV[0].to_i || 2089
@all_boards = {}
@boards_found = 0

find_all ######## DO IT!!!

