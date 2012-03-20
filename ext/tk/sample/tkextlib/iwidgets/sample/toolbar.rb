#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

##########################################
# icon images
editcopy22 = TkPhotoImage.new(:data=><<'EOD')
   R0lGODlhFgAWAIUAAPwCBBQSFPz+/DQyNISChDw6PMzKzMTGxERGRIyKjFxa
   XMTCvKSmpHR2dPz6/Pz29PTq3MS2rPz69MTCxFxWVHx6dJyWjNzSzPz27Pzy
   7Pzu5PTm3NTKvIR+fJyGfHxuZHxqXNTCtPTq5PTi1PTezNS+rExOTFRORMyy
   lPTaxOzWxOzSvNze3NTOxMy2nMyulMyqjAQCBAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAALAAAAAAWABYAAAbY
   QIBwSCwahYGkUnk0BgTQ6IAQaBKfUWhBYKhaAU+CgXAQIAyChLeJzSIQhcH6
   GFaM0QtGY5kstqEODw8QEQELAhJTc08KBBMEFBUWDRcBE1pca20SGBkaEBsc
   AY5maFRIAgoLHRQRHh8gIQFlZnByqA8ZGSIQIyQjJQEmYgJ5p2ACrK4gJx4g
   KIZZAgdeAQ4ZI9kjKSor0AwEjeAs1S0cHAslLi4vMDDRWeRIfEsxMeET4ATy
   VoYLC5fizXEiAR84BeMG+pEm8EsAFhAjSlR4hR6fLxiF0AkCACH+aENyZWF0
   ZWQgYnkgQk1QVG9HSUYgUHJvIHZlcnNpb24gMi41DQqpIERldmVsQ29yIDE5
   OTcsMTk5OC4gQWxsIHJpZ2h0cyByZXNlcnZlZC4NCmh0dHA6Ly93d3cuZGV2
   ZWxjb3IuY29tADs=
EOD

editcut22 = TkPhotoImage.new(:data=><<'EOD')
   R0lGODlhFgAWAIMAAPwCBAQCBAwCBPz+/OTi5JyanOzq7DQyNGxqbAAAAAAA
   AAAAAAAAAAAAAAAAAAAAACH5BAEAAAAALAAAAAAWABYAAARbEMhJq704gxBE
   0Bf3cZo4kRJqBQNRfBucyudgvJS6VaxLzyMa6/bLiWA9HOg4VIIkL5vzuRkc
   pkvRIIAorphJLzBW84WEuRZWp6uaT7J2Sh1Hit3OY/ZO7WvsEQAh/mhDcmVh
   dGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJzaW9uIDIuNQ0KqSBEZXZlbENvciAx
   OTk3LDE5OTguIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQpodHRwOi8vd3d3LmRl
   dmVsY29yLmNvbQA7
EOD

editpaste22 = TkPhotoImage.new(:data=><<'EOD')
   R0lGODlhFgAWAIYAAPwCBBQWFDw6FHRuFGRaBFxSBAQCBAQKBCQiBIx6HPz6
   /NTOfKyiXDQuFOTm5Pz+/Ozu7PTq5Pz63PTyxNTOjKSeRExGLMTGxMzKzNTS
   1NTW1Dw2NKSmpKyqrKSipJyanNzWlLy6ZLSuVIx6FISChIyKhJSSlCQiJLS2
   tDw6NDQyNCQiFCQmHBQSDGRiZHRydGxubHx6dGxqbFxeXGRmZFxaXCwuLOzq
   7KyurHx+fDwmFEQuFCweFCQWDBQODBwaHBweHKSinJSWlOTi5JyepHR2dDw6
   PBQSFNze3ERGRIyKjIyOjISGhPz29Pzy7MS2rMzOzFRWVHx2dHxybDQiFPz2
   7Pzu5PTq3PTm1NTCtJyGdHxuZHxqXPzq3PTaxNS6pFxWVFRKRNS2nPTi1PTS
   tNSulNzOxNSynMymhAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
   LAAAAAAWABYAAAf/gACCgwABAgMEBYSLggaOjgcICQoLDA2Pj4MGDg8QEZ4F
   DxITFBUWFxcYGRobjQ8cHR4fCQ8gCyEiFSMWJCUkJieNEB4dKB4pKissK8wr
   LS4vMDHBAAYQHx8dFx0fJDIzNDU0M+IyHzaNNyg43Ng5Ojs7Ojw9Pj9AMkCN
   DiZB/h9CSOx4QLCgihItqBkYgqIDESElitAYWJCgkQcXjjRCgi1Ihw4BB5LA
   QOLCgyQYHihpUU3DBw5ElpAgAYNixSRJjKjQaECDCRPZPDB5IbIGSQwKLnh4
   wbInLA4kmJB4oaPiAwVNnER40hRK1BIAaVatUZJEFCkmpmjgCeWDCalFe4q4
   oFKwSRUrEa5gycLzwq8lUnPQ4PEgSpYcUZ5o2cIlS1O/JHLEDdfjQZMIVrpg
   weLFy5e+M6WSmBGlxYMYYBRzCaOFi5imHWBIfOEiShLTVjaP6eyFTBmN1TA5
   OvLDjJksWb58OVMGDRqWjAYdmU79SIvpjqJr104nEAAh/mhDcmVhdGVkIGJ5
   IEJNUFRvR0lGIFBybyB2ZXJzaW9uIDIuNQ0KqSBEZXZlbENvciAxOTk3LDE5
   OTguIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQpodHRwOi8vd3d3LmRldmVsY29y
   LmNvbQA7
EOD

editdelete22 = TkPhotoImage.new(:data=><<'EOD')
   R0lGODlhFgAWAIYAAASC/FRSVExKTERCRDw6PDQyNCwuLBweHBwaHAwODAwK
   DAQCBExOTNze3NTW1MTGxLS2tJyanPz+/Ozu7BQSFCwqLDw+POTi5PTu7MzK
   xIR+fCQmJPz6/Oze1NTGvPz69Pzy7Pz29LyyrPy+vPyupPTm1BQWFIQCBPwC
   BMS6rPzSzNTOxPTi1NS+rPTezNzOxPTizOzWxMy2pOzaxMy2nPTaxOzOtMyy
   nOzSvMyqjPx+fOzGpMSihPTq3OzKrOTCpNzKxNTCtAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
   LAAAAAAWABYAAAf8gACCAQIDBAUGBwgJCgsLgpCRAAwNlZYODxALEY+SkAMN
   EqKjEw0UD5yegqCjrRMVEqidkgWhraMWF7GptLa3EgEWFRSOnhW+vxgZEBqz
   kBvItxwdHryRCNGjHyAhHSLOgtgSI60c2yQjJd+eJqEnKK0hJCgnJSngAO0S
   F+8qEvL0VrBogW+BLX4oVKgIyMIFQU8KfDV4R+8FDBcxZBREthAFiRIsOsyg
   sVEUh4Un3pGoUcPGjZInK65QicPlxg8oX5RwqNJGjo0hdJwQ6EIkjRM6dvDY
   CKIHSBc1Ztjw4eOH0oIrsgIJEqSFDBo0cuTgsdSTo7No0xYTZCcQACH+aENy
   ZWF0ZWQgYnkgQk1QVG9HSUYgUHJvIHZlcnNpb24gMi41DQqpIERldmVsQ29y
   IDE5OTcsMTk5OC4gQWxsIHJpZ2h0cyByZXNlcnZlZC4NCmh0dHA6Ly93d3cu
   ZGV2ZWxjb3IuY29tADs=
EOD

text22 = TkPhotoImage.new(:data=><<'EOD')
   R0lGODlhFgAWAIQAAPwCBAQCBBwaHAwKDBQSFLy+vLS2tJSWlBQWFKyqrFRS
   VCwqLDQyNNTS1GxqbFxaXJyanIyOjAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAALAAAAAAWABYAAAVcICCOZGmK
   QSoMaZsShBsQBdAapHvgaIDUqUPJlRjSbAoT0fRDKgODRbF0PLUYjZO2F2Bs
   t9evNix+dsvDlGKK5jraudQb7qbX6a2HEJ+ycyF+LRE8ZTI+fX5oGCEAIf5o
   Q3JlYXRlZCBieSBCTVBUb0dJRiBQcm8gdmVyc2lvbiAyLjUNCqkgRGV2ZWxD
   b3IgMTk5NywxOTk4LiBBbGwgcmlnaHRzIHJlc2VydmVkLg0KaHR0cDovL3d3
   dy5kZXZlbGNvci5jb20AOw==
EOD

##########################################

bmp_dir = File.join(File.dirname(File.expand_path(__FILE__)),
                    '../catalog_demo/images')

##########################################

status_var = TkVariable.new
radio_var  = TkVariable.new
check_var1 = TkVariable.new
check_var2 = TkVariable.new

tb = Tk::Iwidgets::Toolbar.new(:helpvariable=>status_var)

##########################################

tb.add(:button, :helpstr=>'Copy It', :image=>editcopy22,
       :balloonstr=>'Copy', :command=>proc{puts 'Copy It'})

tb.add(:button, :helpstr=>'Cut It', :image=>editcut22,
       :balloonstr=>'Cut', :command=>proc{puts 'Cut It'})

tb.add(:button, :helpstr=>'Paste It', :image=>editpaste22,
       :balloonstr=>'Paste', :command=>proc{puts 'Paste It'})

tb.add(:button, :helpstr=>'Delete It', :image=>editdelete22,
       :balloonstr=>'Delete', :command=>proc{puts 'Delete It'})

#--------------------------------

tb.add(:frame, :borderwidth=>1, :width=>10, :height=>10)

#--------------------------------

tb.add(:radiobutton, :variable=>radio_var, :value=>'Box',
       :bitmap=>"@#{bmp_dir}/box.xbm",
       :helpstr=>'Radio Button #1', :balloonstr=>'Radio',
       :command=>proc{puts 'Radio Button "Box"'})

tb.add(:radiobutton, :variable=>radio_var, :value=>'Line',
       :bitmap=>"@#{bmp_dir}/line.xbm",
       :helpstr=>'Radio Button #2', :balloonstr=>'Radio',
       :command=>proc{puts 'Radio Button "Line"'})

tb.add(:radiobutton, :variable=>radio_var, :value=>'Oval',
       :bitmap=>"@#{bmp_dir}/oval.xbm",
       :helpstr=>'Radio Button #3', :balloonstr=>'Radio',
       :command=>proc{puts 'Radio Button "Oval"'})

#--------------------------------

tb.add(:frame, :borderwidth=>1, :width=>10, :height=>10)

#--------------------------------

tb.add(:checkbutton, :variable=>check_var1, :onvalue=>'yes', :offvalue=>'no',
       :image=>text22, :command=>proc{puts 'Checkbutton 1'})

tb.add(:checkbutton, :variable=>check_var2, :onvalue=>'yes', :offvalue=>'no',
       :bitmap=>"@#{bmp_dir}/points.xbm", :command=>proc{puts 'Checkbutton 2'})

tb.pack(:side=>:top, :anchor=>:nw)

Tk.mainloop
