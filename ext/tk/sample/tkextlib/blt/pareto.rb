#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/blt'

# Example of a pareto chart.
#
# The pareto chart mixes line and bar elements in the same graph.
# Each processing operating is represented by a bar element.  The
# total accumulated defects is displayed with a single line element.
b = Tk::BLT::Barchart.new(:title=>'Defects Found During Inspection',
                          :font=>'Helvetica 12', :plotpady=>[12, 4],
                          :width=>'6i', :height=>'5i')
Tk::BLT::Table.add(Tk.root, b, :fill=>:both)

data = [
  ["Spot Weld",  82,   'yellow'],
  ["Lathe",      49,   'orange'],
  ["Gear Cut",   38,   'green'],
  ["Drill",      24,   'blue'],
  ["Grind",      17,   'red'],
  ["Lapping",    12,   'brown'],
  ["Press",       8,   'purple'],
  ["De-burr",     4,   'pink'],
  ["Packaging",   3,   'cyan'],
  ["Other",      12,   'magenta']
]

# Create an X-Y graph line element to trace the accumulated defects.
b.line_create('accum', :label=>'', :symbol=>:none, :color=>'red')

# Define a bitmap to be used to stipple the background of each bar.
pattern1 = Tk::BLT::Bitmap.define([ [4, 4], [1, 2, 4, 8] ])

# For each process, create a bar element to display the magnitude.
count = 0
sum   = 0
ydata = [0]
xdata = [0]
labels = []

data.each{|label, value, color|
  count += 1
  b.element_create(label, :xdata=>count, :ydata=>value, :foreground=>color,
                   :relief=>:solid, :borderwidth=>1, :stipple=>pattern1,
                   :background=>'lightblue')
  labels[count] = label
  # Get the total number of defects.
  sum += value
  ydata << sum
  xdata << count
}

# Configure the coordinates of the accumulated defects,
# now that we know what they are.
b.element_configure('accum', :xdata=>xdata, :ydata=>ydata)

# Add text markers to label the percentage of total at each point.
xdata.zip(ydata){|x, y|
  percent = (y * 100.0) / sum
  if x == 0
    text = ' 0%'
  else
    text = '%.1f' % percent
  end
  b.marker_create(:text, :coords=>[x, y], :text=>text, :font=>'Helvetica 10',
                  :foreground=>'red4', :anchor=>:center, :yoffset=>-5)
}

# Display an auxillary y-axis for percentages.
b.axis_configure('y2', :hide=>false, :min=>0.0, :max=>100.0,
                 :title=>'Percentage')

# Title the y-axis
b.axis_configure('y', :title=>'Defects')

# Configure the x-axis to display the process names, instead of numbers.
b.axis_configure('x', :title=>'Process', :rotate=>90, :subdivisions=>0,
                 :command=>proc{|w, val|
                   val = val.round
                   labels[val]? labels[val]: val
                  })

# No legend needed.
b.legend_configure(:hide=>true)

# Configure the grid lines.
b.gridline_configure(:mapx=>:x, :color=>'lightblue')

Tk.mainloop
