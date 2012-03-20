require 'test/unit'
require 'date'

class TestDateParse < Test::Unit::TestCase

  def test__parse
    [
     # ctime(3), asctime(3)
     [['Sat Aug 28 02:55:50 1999',false],[1999,8,28,2,55,50,nil,nil,6]],
     [['Sat Aug 28 02:55:50 02',false],[2,8,28,2,55,50,nil,nil,6]],
     [['Sat Aug 28 02:55:50 02',true],[2002,8,28,2,55,50,nil,nil,6]],
     [['Sat Aug 28 02:55:50 0002',false],[2,8,28,2,55,50,nil,nil,6]],
     [['Sat Aug 28 02:55:50 0002',true],[2,8,28,2,55,50,nil,nil,6]],

     # date(1)
     [['Sat Aug 28 02:29:34 JST 1999',false],[1999,8,28,2,29,34,'JST',9*3600,6]],
     [['Sat Aug 28 02:29:34 MET DST 1999',false],[1999,8,28,2,29,34,'MET DST',2*3600,6]],
     [['Sat Aug 28 02:29:34 AMT 1999',false],[1999,8,28,2,29,34,'AMT',nil,6]],
     [['Sat Aug 28 02:29:34 PMT 1999',false],[1999,8,28,2,29,34,'PMT',nil,6]],
     [['Sat Aug 28 02:29:34 PMT -1999',false],[-1999,8,28,2,29,34,'PMT',nil,6]],

     [['Sat Aug 28 02:29:34 JST 02',false],[2,8,28,2,29,34,'JST',9*3600,6]],
     [['Sat Aug 28 02:29:34 JST 02',true],[2002,8,28,2,29,34,'JST',9*3600,6]],
     [['Sat Aug 28 02:29:34 JST 0002',false],[2,8,28,2,29,34,'JST',9*3600,6]],
     [['Sat Aug 28 02:29:34 JST 0002',true],[2,8,28,2,29,34,'JST',9*3600,6]],

     [['Sat Aug 28 02:29:34 GMT+09 0002',false],[2,8,28,2,29,34,'GMT+09',9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT+0900 0002',false],[2,8,28,2,29,34,'GMT+0900',9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT+09:00 0002',false],[2,8,28,2,29,34,'GMT+09:00',9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT-09 0002',false],[2,8,28,2,29,34,'GMT-09',-9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT-0900 0002',false],[2,8,28,2,29,34,'GMT-0900',-9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT-09:00 0002',false],[2,8,28,2,29,34,'GMT-09:00',-9*3600,6]],
     [['Sat Aug 28 02:29:34 GMT-090102 0002',false],[2,8,28,2,29,34,'GMT-090102',-9*3600-60-2,6]],
     [['Sat Aug 28 02:29:34 GMT-09:01:02 0002',false],[2,8,28,2,29,34,'GMT-09:01:02',-9*3600-60-2,6]],

     [['Sat Aug 28 02:29:34 GMT Standard Time 2000',false],[2000,8,28,2,29,34,'GMT Standard Time',0*3600,6]],
     [['Sat Aug 28 02:29:34 Mountain Standard Time 2000',false],[2000,8,28,2,29,34,'Mountain Standard Time',-7*3600,6]],
     [['Sat Aug 28 02:29:34 Mountain Daylight Time 2000',false],[2000,8,28,2,29,34,'Mountain Daylight Time',-6*3600,6]],
     [['Sat Aug 28 02:29:34 Mexico Standard Time 2000',false],[2000,8,28,2,29,34,'Mexico Standard Time',-6*3600,6]],
#     [['Sat Aug 28 02:29:34 Mexico Standard Time 2 2000',false],[2000,8,28,2,29,34,'Mexico Standard Time 2',-7*3600,6]], # cp
     [['Sat Aug 28 02:29:34 E. Australia Standard Time 2000',false],[2000,8,28,2,29,34,'E. Australia Standard Time',10*3600,6]],

     # part of iso 8601
     [['1999-05-23 23:55:21',false],[1999,5,23,23,55,21,nil,nil,nil]],
     [['1999-05-23 23:55:21+0900',false],[1999,5,23,23,55,21,'+0900',9*3600,nil]],
     [['1999-05-23 23:55:21-0900',false],[1999,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['1999-05-23 23:55:21+09:00',false],[1999,5,23,23,55,21,'+09:00',9*3600,nil]],
     [['1999-05-23T23:55:21-09:00',false],[1999,5,23,23,55,21,'-09:00',-9*3600,nil]],
     [['1999-05-23 23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],
     [['1999-05-23T23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],
     [['-1999-05-23T23:55:21Z',false],[-1999,5,23,23,55,21,'Z',0,nil]],
     [['-1999-05-23T23:55:21Z',true],[-1999,5,23,23,55,21,'Z',0,nil]],
     [['19990523T23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],

     [['+011985-04-12',false],[11985,4,12,nil,nil,nil,nil,nil,nil]],
     [['+011985-04-12T10:15:30',false],[11985,4,12,10,15,30,nil,nil,nil]],
     [['-011985-04-12',false],[-11985,4,12,nil,nil,nil,nil,nil,nil]],
     [['-011985-04-12T10:15:30',false],[-11985,4,12,10,15,30,nil,nil,nil]],

     [['02-04-12',false],[2,4,12,nil,nil,nil,nil,nil,nil]],
     [['02-04-12',true],[2002,4,12,nil,nil,nil,nil,nil,nil]],
     [['0002-04-12',false],[2,4,12,nil,nil,nil,nil,nil,nil]],
     [['0002-04-12',true],[2,4,12,nil,nil,nil,nil,nil,nil]],

     [['19990523',true],[1999,5,23,nil,nil,nil,nil,nil,nil]],
     [['-19990523',true],[-1999,5,23,nil,nil,nil,nil,nil,nil]],
     [['990523',true],[1999,5,23,nil,nil,nil,nil,nil,nil]],
     [['0523',false],[nil,5,23,nil,nil,nil,nil,nil,nil]],
     [['23',false],[nil,nil,23,nil,nil,nil,nil,nil,nil]],

     [['19990523 235521',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['990523 235521',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['0523 2355',false],[nil,5,23,23,55,nil,nil,nil,nil]],
     [['23 2355',false],[nil,nil,23,23,55,nil,nil,nil,nil]],

     [['19990523T235521',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['990523T235521',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['19990523T235521.99',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['990523T235521.99',true],[1999,5,23,23,55,21,nil,nil,nil]],
     [['0523T2355',false],[nil,5,23,23,55,nil,nil,nil,nil]],

     [['19990523T235521+0900',true],[1999,5,23,23,55,21,'+0900',9*3600,nil]],
     [['990523T235521-0900',true],[1999,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['19990523T235521.99+0900',true],[1999,5,23,23,55,21,'+0900',9*3600,nil]],
     [['990523T235521.99-0900',true],[1999,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['0523T2355Z',false],[nil,5,23,23,55,nil,'Z',0,nil]],

     [['19990523235521.123456+0900',true],[1999,5,23,23,55,21,'+0900',9*3600,nil]],
     [['19990523235521.123456-0900',true],[1999,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['19990523235521,123456+0900',true],[1999,5,23,23,55,21,'+0900',9*3600,nil]],
     [['19990523235521,123456-0900',true],[1999,5,23,23,55,21,'-0900',-9*3600,nil]],

     [['990523235521,123456-0900',false],[99,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['0523235521,123456-0900',false],[nil,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['23235521,123456-0900',false],[nil,nil,23,23,55,21,'-0900',-9*3600,nil]],
     [['235521,123456-0900',false],[nil,nil,nil,23,55,21,'-0900',-9*3600,nil]],
     [['5521,123456-0900',false],[nil,nil,nil,nil,55,21,'-0900',-9*3600,nil]],
     [['21,123456-0900',false],[nil,nil,nil,nil,nil,21,'-0900',-9*3600,nil]],

     [['3235521,123456-0900',false],[nil,nil,3,23,55,21,'-0900',-9*3600,nil]],
     [['35521,123456-0900',false],[nil,nil,nil,3,55,21,'-0900',-9*3600,nil]],
     [['521,123456-0900',false],[nil,nil,nil,nil,5,21,'-0900',-9*3600,nil]],

     # reversed iso 8601 (?)
     [['23-05-1999',false],[1999,5,23,nil,nil,nil,nil,nil,nil]],
     [['23-05-1999 23:55:21',false],[1999,5,23,23,55,21,nil,nil,nil]],
     [['23-05--1999 23:55:21',false],[-1999,5,23,23,55,21,nil,nil,nil]],
     [["23-05-'99",false],[99,5,23,nil,nil,nil,nil,nil,nil]],
     [["23-05-'99",true],[1999,5,23,nil,nil,nil,nil,nil,nil]],

     # broken iso 8601 (?)
#     [['1999-05-23T235521Z',false],[1999,5,23,23,55,21,'Z',0,nil]], # cp
     [['19990523T23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],
     [['19990523235521.1234-100',true],[1999,5,23,23,55,21,'-100',-1*3600,nil]],
     [['19990523235521.1234-10',true],[1999,5,23,23,55,21,'-10',-10*3600,nil]],

     # part of jis x0301
     [['M11.05.23',false],[1878,5,23,nil,nil,nil,nil,nil,nil]],
     [['T11.05.23 23:55:21+0900',false],[1922,5,23,23,55,21,'+0900',9*3600,nil]],
     [['S11.05.23 23:55:21-0900',false],[1936,5,23,23,55,21,'-0900',-9*3600,nil]],
     [['S40.05.23 23:55:21+09:00',false],[1965,5,23,23,55,21,'+09:00',9*3600,nil]],
     [['S40.05.23T23:55:21-09:00',false],[1965,5,23,23,55,21,'-09:00',-9*3600,nil]],
     [['H11.05.23 23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],
     [['H11.05.23T23:55:21Z',false],[1999,5,23,23,55,21,'Z',0,nil]],

     # ofx date
     [['19990523235521',false],[1999,5,23,23,55,21,nil,nil,nil]],
     [['19990523235521.123',false],[1999,5,23,23,55,21,nil,nil,nil]],
     [['19990523235521.123[-9]',false],[1999,5,23,23,55,21,'-9',-(9*3600),nil]],
     [['19990523235521.123[+9]',false],[1999,5,23,23,55,21,'+9',+(9*3600),nil]],
     [['19990523235521.123[9]',false],[1999,5,23,23,55,21,'9',+(9*3600),nil]],
     [['19990523235521.123[-9.50]',false],[1999,5,23,23,55,21,'-9.50',-(9*3600+30*60),nil]],
     [['19990523235521.123[+9.50]',false],[1999,5,23,23,55,21,'+9.50',+(9*3600+30*60),nil]],
     [['19990523235521.123[-5:EST]',false],[1999,5,23,23,55,21,'EST',-5*3600,nil]],
     [['19990523235521.123[+9:JST]',false],[1999,5,23,23,55,21,'JST',9*3600,nil]],
     [['19990523235521.123[+12:XXX YYY ZZZ]',false],[1999,5,23,23,55,21,'XXX YYY ZZZ',12*3600,nil]],
#     [['235521',false],[nil,nil,nil,23,55,21,nil,nil,nil]], # cp
     [['235521.123',false],[nil,nil,nil,23,55,21,nil,nil,nil]],
     [['235521.123[-9]',false],[nil,nil,nil,23,55,21,'-9',-9*3600,nil]],
     [['235521.123[+9]',false],[nil,nil,nil,23,55,21,'+9',+9*3600,nil]],
     [['235521.123[-5:EST]',false],[nil,nil,nil,23,55,21,'EST',-5*3600,nil]],
     [['235521.123[+9:JST]',false],[nil,nil,nil,23,55,21,'JST',+9*3600,nil]],

     # rfc 2822
     [['Sun, 22 Aug 1999 00:45:29 -0400',false],[1999,8,22,0,45,29,'-0400',-4*3600,0]],
     [['Sun, 22 Aug 1999 00:45:29 -9959',false],[1999,8,22,0,45,29,'-9959',-(99*3600+59*60),0]],
     [['Sun, 22 Aug 1999 00:45:29 +9959',false],[1999,8,22,0,45,29,'+9959',+(99*3600+59*60),0]],
     [['Sun, 22 Aug 05 00:45:29 -0400',true],[2005,8,22,0,45,29,'-0400',-4*3600,0]],
     [['Sun, 22 Aug 49 00:45:29 -0400',true],[2049,8,22,0,45,29,'-0400',-4*3600,0]],
#     [['Sun, 22 Aug 50 00:45:29 -0400',true],[1950,8,22,0,45,29,'-0400',-4*3600,0]],
#     [['Sun, 22 Aug 111 00:45:29 -0400',true],[2011,8,22,0,45,29,'-0400',-4*3600,0]],
     [['Sun, 22 Aug 1999 00:45:29 GMT',false],[1999,8,22,0,45,29,'GMT',0,0]],
     [["Sun,\00022\r\nAug\r\n1999\r\n00:45:29\r\nGMT",false],[1999,8,22,0,45,29,'GMT',0,0]],
     [['Sun, 22 Aug 1999 00:45 GMT',false],[1999,8,22,0,45,nil,'GMT',0,0]],
     [['Sun, 22 Aug -1999 00:45 GMT',false],[-1999,8,22,0,45,nil,'GMT',0,0]],
     [['Sun, 22 Aug 99 00:45:29 UT',true],[1999,8,22,0,45,29,'UT',0,0]],
     [['Sun, 22 Aug 0099 00:45:29 UT',true],[99,8,22,0,45,29,'UT',0,0]],

     # rfc 850, obsoleted by rfc 1036
     [['Tuesday, 02-Mar-99 11:20:32 GMT',true],[1999,3,2,11,20,32,'GMT',0,2]],

     # W3C Working Draft - XForms - 4.8 Time
     [['2000-01-31 13:20:00-5',false],[2000,1,31,13,20,0,'-5',-5*3600,nil]],

     # [-+]\d+.\d+
     [['2000-01-31 13:20:00-5.5',false],[2000,1,31,13,20,0,'-5.5',-5*3600-30*60,nil]],
     [['2000-01-31 13:20:00-5,5',false],[2000,1,31,13,20,0,'-5,5',-5*3600-30*60,nil]],
     [['2000-01-31 13:20:00+3.5',false],[2000,1,31,13,20,0,'+3.5',3*3600+30*60,nil]],
     [['2000-01-31 13:20:00+3,5',false],[2000,1,31,13,20,0,'+3,5',3*3600+30*60,nil]],

     # mil
     [['2000-01-31 13:20:00 Z',false],[2000,1,31,13,20,0,'Z',0*3600,nil]],
     [['2000-01-31 13:20:00 H',false],[2000,1,31,13,20,0,'H',8*3600,nil]],
     [['2000-01-31 13:20:00 M',false],[2000,1,31,13,20,0,'M',12*3600,nil]],
     [['2000-01-31 13:20 M',false],[2000,1,31,13,20,nil,'M',12*3600,nil]],
     [['2000-01-31 13:20:00 S',false],[2000,1,31,13,20,0,'S',-6*3600,nil]],
     [['2000-01-31 13:20:00 A',false],[2000,1,31,13,20,0,'A',1*3600,nil]],
     [['2000-01-31 13:20:00 P',false],[2000,1,31,13,20,0,'P',-3*3600,nil]],

     # dot
     [['1999.5.2',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['1999.05.02',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['-1999.05.02',false],[-1999,5,2,nil,nil,nil,nil,nil,nil]],
#     [['05.02',false],[nil,5,2,nil,nil,nil,nil,nil,nil]], # not support
#     [[' 5. 2',false],[nil,5,2,nil,nil,nil,nil,nil,nil]], # not support

     [['0099.5.2',false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [['0099.5.2',true],[99,5,2,nil,nil,nil,nil,nil,nil]],

     [["'99.5.2",false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [["'99.5.2",true],[1999,5,2,nil,nil,nil,nil,nil,nil]],

     # reversed dot
     [['2.5.1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['02.05.1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['02.05.-1999',false],[-1999,5,2,nil,nil,nil,nil,nil,nil]],

     [['2.5.0099',false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [['2.5.0099',true],[99,5,2,nil,nil,nil,nil,nil,nil]],

     [["2.5.'99",false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [["2.5.'99",true],[1999,5,2,nil,nil,nil,nil,nil,nil]],

     # vms
     [['08-DEC-1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['31-JAN-1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['31-JAN--1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],

     [['08-DEC-88',false],[88,12,8,nil,nil,nil,nil,nil,nil]],
     [['08-DEC-88',true],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['08-DEC-0088',false],[88,12,8,nil,nil,nil,nil,nil,nil]],
     [['08-DEC-0088',true],[88,12,8,nil,nil,nil,nil,nil,nil]],

     # swaped vms
     [['DEC-08-1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['JAN-31-1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['JAN-31--1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['JAN-1999',false],[1999,1,nil,nil,nil,nil,nil,nil,nil]],
     [['JAN--1999',false],[-1999,1,nil,nil,nil,nil,nil,nil,nil]],

     # reversed vms
     [['1988-DEC-08',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['1999-JAN-31',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['-1999-JAN-31',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],

     [['0088-DEC-08',false],[88,12,8,nil,nil,nil,nil,nil,nil]],
     [['0088-DEC-08',true],[88,12,8,nil,nil,nil,nil,nil,nil]],

     [["'88/12/8",false],[88,12,8,nil,nil,nil,nil,nil,nil]],
     [["'88/12/8",true],[1988,12,8,nil,nil,nil,nil,nil,nil]],

     # non-spaced eu
     [['08/dec/1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['31/jan/1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['31/jan/-1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['08.dec.1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['31.jan.1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['31.jan.-1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],

     # non-spaced us
     [['dec/08/1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['jan/31/1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan/31/-1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan/31',false],[nil,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan/1988',false],[1988,1,nil,nil,nil,nil,nil,nil,nil]],
     [['dec.08.1988',false],[1988,12,8,nil,nil,nil,nil,nil,nil]],
     [['jan.31.1999',false],[1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan.31.-1999',false],[-1999,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan.31',false],[nil,1,31,nil,nil,nil,nil,nil,nil]],
     [['jan.1988',false],[1988,1,nil,nil,nil,nil,nil,nil,nil]],

     # month and day of month
     [['Jan 1',false],[nil,1,1,nil,nil,nil,nil,nil,nil]],
     [['Jul 11',false],[nil,7,11,nil,nil,nil,nil,nil,nil]],
     [['July 11',false],[nil,7,11,nil,nil,nil,nil,nil,nil]],
     [['Sept 23',false],[nil,9,23,nil,nil,nil,nil,nil,nil]],
     [['Sep. 23',false],[nil,9,23,nil,nil,nil,nil,nil,nil]],
     [['Sept. 23',false],[nil,9,23,nil,nil,nil,nil,nil,nil]],
     [['September 23',false],[nil,9,23,nil,nil,nil,nil,nil,nil]],
     [['October 1st',false],[nil,10,1,nil,nil,nil,nil,nil,nil]],
     [['October 23rd',false],[nil,10,23,nil,nil,nil,nil,nil,nil]],
     [['October 25th 1999',false],[1999,10,25,nil,nil,nil,nil,nil,nil]],
     [['October 25th -1999',false],[-1999,10,25,nil,nil,nil,nil,nil,nil]],
     [['october 25th 1999',false],[1999,10,25,nil,nil,nil,nil,nil,nil]],
     [['OCTOBER 25th 1999',false],[1999,10,25,nil,nil,nil,nil,nil,nil]],
     [['oCtoBer 25th 1999',false],[1999,10,25,nil,nil,nil,nil,nil,nil]],
     [['aSep 23',false],[nil,nil,23,nil,nil,nil,nil,nil,nil]],

     # month and year
     [['Sept 1990',false],[1990,9,nil,nil,nil,nil,nil,nil,nil]],
     [["Sept '90",false],[90,9,nil,nil,nil,nil,nil,nil,nil]],
     [["Sept '90",true],[1990,9,nil,nil,nil,nil,nil,nil,nil]],
     [['1990/09',false],[1990,9,nil,nil,nil,nil,nil,nil,nil]],
     [['09/1990',false],[1990,9,nil,nil,nil,nil,nil,nil,nil]],
     [["aSep '90",false],[90,nil,nil,nil,nil,nil,nil,nil,nil]],

     # year
     [["'90",false],[90,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["'90",true],[1990,nil,nil,nil,nil,nil,nil,nil,nil]],

     # month
     [['Jun',false],[nil,6,nil,nil,nil,nil,nil,nil,nil]],
     [['June',false],[nil,6,nil,nil,nil,nil,nil,nil,nil]],
     [['Sep',false],[nil,9,nil,nil,nil,nil,nil,nil,nil]],
     [['Sept',false],[nil,9,nil,nil,nil,nil,nil,nil,nil]],
     [['September',false],[nil,9,nil,nil,nil,nil,nil,nil,nil]],
     [['aSep',false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],

     # day of month
     [['1st',false],[nil,nil,1,nil,nil,nil,nil,nil,nil]],
     [['2nd',false],[nil,nil,2,nil,nil,nil,nil,nil,nil]],
     [['3rd',false],[nil,nil,3,nil,nil,nil,nil,nil,nil]],
     [['4th',false],[nil,nil,4,nil,nil,nil,nil,nil,nil]],
     [['29th',false],[nil,nil,29,nil,nil,nil,nil,nil,nil]],
     [['31st',false],[nil,nil,31,nil,nil,nil,nil,nil,nil]],
     [['1sta',false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],

     # era
     [['Sat Aug 28 02:29:34 GMT CE 2000',false],[2000,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT C.E. 2000',false],[2000,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT BCE 2000',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT B.C.E. 2000',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT AD 2000',false],[2000,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT A.D. 2000',false],[2000,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT BC 2000',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT B.C. 2000',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT 2000 BC',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT 2000 BCE',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT 2000 B.C.',false],[-1999,8,28,2,29,34,'GMT',0,6]],
     [['Sat Aug 28 02:29:34 GMT 2000 B.C.E.',false],[-1999,8,28,2,29,34,'GMT',0,6]],

     # collection
#     [['le ler juillet 1982',false],[1982,7,1,nil,nil,nil,nil,nil,nil]], # bih 1982
#     [['30 June 1982 , 23h 59m 59s',false],[1982,6,30,23,59,59,nil,nil,nil]], # bih 1982
     [['Tuesday, May 18, 1999 Published at 13:36 GMT 14:36 UK',false],[1999,5,18,13,36,nil,'GMT',0,2]], # bbc.co.uk
     [['July 20, 2000 Web posted at: 3:37 p.m. EDT (1937 GMT)',false],[2000,7,20,15,37,nil,'EDT',-4*3600,nil]], # cnn.com
     [['12:54 p.m. EDT, September 11, 2006',false],[2006,9,11,12,54,nil,'EDT',-4*3600,nil]], # cnn.com
     [['February 04, 2001 at 10:59 AM PST',false],[2001,2,4,10,59,nil,'PST',-8*3600,nil]], # old amazon.com
     [['Monday May 08, @01:55PM',false],[nil,5,8,13,55,nil,nil,nil,1]], # slashdot.org
     [['06.June 2005',false],[2005,6,6,nil,nil,nil,nil,nil,nil]], # dhl.com

     # etc.
     [['8:00 pm lt',false],[nil,nil,nil,20,0,nil,'lt',nil,nil]],
     [['4:00 AM, Jan. 12, 1990',false],[1990,1,12,4,0,nil,nil,nil,nil]],
     [['Jan. 12 4:00 AM 1990',false],[1990,1,12,4,0,nil,nil,nil,nil]],
#     [['Jan. 12 4:00 -1990',false],[-1990,1,12,4,0,nil,nil,nil,nil]], # cp
     [['1990-01-12 04:00:00+00',false],[1990,1,12,4,0,0,'+00',0,nil]],
     [['1990-01-11 20:00:00-08',false],[1990,1,11,20,0,0,'-08',-8*3600,nil]],
     [['1990/01/12 04:00:00',false],[1990,1,12,4,0,0,nil,nil,nil]],
#     [['Thu Jan 11 20:00:00 1990 LT',false], [1990,1,11,20,0,0,'LT',nil,4]], # cp
     [['Thu Jan 11 20:00:00 PST 1990',false],[1990,1,11,20,0,0,'PST',-8*3600,4]],
     [['Fri Jan 12 04:00:00 GMT 1990',false],[1990,1,12,4,0,0,'GMT',0,5]],
     [['Thu, 11 Jan 1990 20:00:00 -0800',false],[1990,1,11,20,0,0,'-0800',-8*3600,4]],
     [['12-January-1990, 04:00 WET',false],[1990,1,12,4,0,nil,'WET',0*3600,nil]],
     [['jan 2 3 am +4 5',false],[5,1,2,3,nil,nil,'+4',4*3600,nil]],
     [['jan 2 3 am +4 5',true],[2005,1,2,3,nil,nil,'+4',4*3600,nil]],
     [['fri1feb3bc4pm+5',false],[-2,2,1,16,nil,nil,'+5',5*3600,5]],
     [['fri1feb3bc4pm+5',true],[-2,2,1,16,nil,nil,'+5',5*3600,5]],
     [['03 feb 1st',false],[03,2,1,nil,nil,nil,nil,nil,nil]],

     # apostrophe
     [["July 4, '79",true],[1979,7,4,nil,nil,nil,nil,nil,nil]],
     [["4th July '79",true],[1979,7,4,nil,nil,nil,nil,nil,nil]],

     # day of week
     [['Sunday',false],[nil,nil,nil,nil,nil,nil,nil,nil,0]],
     [['Mon',false],[nil,nil,nil,nil,nil,nil,nil,nil,1]],
     [['Tue',false],[nil,nil,nil,nil,nil,nil,nil,nil,2]],
     [['Wed',false],[nil,nil,nil,nil,nil,nil,nil,nil,3]],
     [['Thurs',false],[nil,nil,nil,nil,nil,nil,nil,nil,4]],
     [['Friday',false],[nil,nil,nil,nil,nil,nil,nil,nil,5]],
     [['Sat.',false],[nil,nil,nil,nil,nil,nil,nil,nil,6]],
     [['sat.',false],[nil,nil,nil,nil,nil,nil,nil,nil,6]],
     [['SAT.',false],[nil,nil,nil,nil,nil,nil,nil,nil,6]],
     [['sAt.',false],[nil,nil,nil,nil,nil,nil,nil,nil,6]],
#     [['su',false],[nil,nil,nil,nil,nil,nil,nil,nil,0]],
#     [['mo',false],[nil,nil,nil,nil,nil,nil,nil,nil,1]],

     # time
     [['09:55',false],[nil,nil,nil,9,55,nil,nil,nil,nil]],
     [['09:55:30',false],[nil,nil,nil,9,55,30,nil,nil,nil]],
     [['09:55:30am',false],[nil,nil,nil,9,55,30,nil,nil,nil]],
     [['09:55:30pm',false],[nil,nil,nil,21,55,30,nil,nil,nil]],
     [['09:55:30a.m.',false],[nil,nil,nil,9,55,30,nil,nil,nil]],
     [['09:55:30p.m.',false],[nil,nil,nil,21,55,30,nil,nil,nil]],
     [['09:55:30pm GMT',false],[nil,nil,nil,21,55,30,'GMT',0,nil]],
     [['09:55:30p.m. GMT',false],[nil,nil,nil,21,55,30,'GMT',0,nil]],
     [['09:55+0900',false],[nil,nil,nil,9,55,nil,'+0900',9*3600,nil]],
     [['09 AM',false],[nil,nil,nil,9,nil,nil,nil,nil,nil]],
     [['09am',false],[nil,nil,nil,9,nil,nil,nil,nil,nil]],
     [['09 A.M.',false],[nil,nil,nil,9,nil,nil,nil,nil,nil]],
     [['09 PM',false],[nil,nil,nil,21,nil,nil,nil,nil,nil]],
     [['09pm',false],[nil,nil,nil,21,nil,nil,nil,nil,nil]],
     [['09 P.M.',false],[nil,nil,nil,21,nil,nil,nil,nil,nil]],

     [['9h22m23s',false],[nil,nil,nil,9,22,23,nil,nil,nil]],
     [['9h 22m 23s',false],[nil,nil,nil,9,22,23,nil,nil,nil]],
     [['9h22m',false],[nil,nil,nil,9,22,nil,nil,nil,nil]],
     [['9h 22m',false],[nil,nil,nil,9,22,nil,nil,nil,nil]],
     [['9h',false],[nil,nil,nil,9,nil,nil,nil,nil,nil]],
     [['9h 22m 23s am',false],[nil,nil,nil,9,22,23,nil,nil,nil]],
     [['9h 22m 23s pm',false],[nil,nil,nil,21,22,23,nil,nil,nil]],
     [['9h 22m am',false],[nil,nil,nil,9,22,nil,nil,nil,nil]],
     [['9h 22m pm',false],[nil,nil,nil,21,22,nil,nil,nil,nil]],
     [['9h am',false],[nil,nil,nil,9,nil,nil,nil,nil,nil]],
     [['9h pm',false],[nil,nil,nil,21,nil,nil,nil,nil,nil]],

     [['00:00',false],[nil,nil,nil,0,0,nil,nil,nil,nil]],
     [['01:00',false],[nil,nil,nil,1,0,nil,nil,nil,nil]],
     [['11:00',false],[nil,nil,nil,11,0,nil,nil,nil,nil]],
     [['12:00',false],[nil,nil,nil,12,0,nil,nil,nil,nil]],
     [['13:00',false],[nil,nil,nil,13,0,nil,nil,nil,nil]],
     [['23:00',false],[nil,nil,nil,23,0,nil,nil,nil,nil]],
     [['24:00',false],[nil,nil,nil,24,0,nil,nil,nil,nil]],

     [['00:00 AM',false],[nil,nil,nil,0,0,nil,nil,nil,nil]],
     [['12:00 AM',false],[nil,nil,nil,0,0,nil,nil,nil,nil]],
     [['01:00 AM',false],[nil,nil,nil,1,0,nil,nil,nil,nil]],
     [['11:00 AM',false],[nil,nil,nil,11,0,nil,nil,nil,nil]],
     [['00:00 PM',false],[nil,nil,nil,12,0,nil,nil,nil,nil]],
     [['12:00 PM',false],[nil,nil,nil,12,0,nil,nil,nil,nil]],
     [['01:00 PM',false],[nil,nil,nil,13,0,nil,nil,nil,nil]],
     [['11:00 PM',false],[nil,nil,nil,23,0,nil,nil,nil,nil]],

     # pick up the rest
     [['2000-01-02 1',false],[2000,1,2,1,nil,nil,nil,nil,nil]],
     [['2000-01-02 23',false],[2000,1,2,23,nil,nil,nil,nil,nil]],
     [['2000-01-02 24',false],[2000,1,2,24,nil,nil,nil,nil,nil]],
     [['1 03:04:05',false],[nil,nil,1,3,4,5,nil,nil,nil]],
     [['02 03:04:05',false],[nil,nil,2,3,4,5,nil,nil,nil]],
     [['31 03:04:05',false],[nil,nil,31,3,4,5,nil,nil,nil]],

     # null, space
     [['',false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\s",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\s" * 10, true],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\t",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\n",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\v",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\f",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\r",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["\t\n\v\f\r\s",false],[nil,nil,nil,nil,nil,nil,nil,nil,nil]],
     [["1999-05-23\t\n\v\f\r\s21:34:56",false],[1999,5,23,21,34,56,nil,nil,nil]],
     [["1999-05-23\n\n\n\n\n\n21:34:56",false],[1999,5,23,21,34,56,nil,nil,nil]],
    ].each do |x,y|
      h = Date._parse(*x)
      a = h.values_at(:year,:mon,:mday,:hour,:min,:sec,:zone,:offset,:wday)
      if y[1] == -1
	a[1] = -1
	a[2] = h[:yday]
      end
      assert_equal(y, a, [x, y, a].inspect)
    end
  end

  def test__parse_slash_exp
    [
     # little
     [['2/5/1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['02/05/1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['02/05/-1999',false],[-1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['05/02',false],[nil,5,2,nil,nil,nil,nil,nil,nil]],
     [[' 5/ 2',false],[nil,5,2,nil,nil,nil,nil,nil,nil]],

     [["2/5/'99",true],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['2/5/0099',false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [['2/5/0099',true],[99,5,2,nil,nil,nil,nil,nil,nil]],

     [['2/5 1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['2/5-1999',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['2/5--1999',false],[-1999,5,2,nil,nil,nil,nil,nil,nil]],

     # big
     [['99/5/2',false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [['99/5/2',true],[1999,5,2,nil,nil,nil,nil,nil,nil]],

     [['1999/5/2',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['1999/05/02',false],[1999,5,2,nil,nil,nil,nil,nil,nil]],
     [['-1999/05/02',false],[-1999,5,2,nil,nil,nil,nil,nil,nil]],

     [['0099/5/2',false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [['0099/5/2',true],[99,5,2,nil,nil,nil,nil,nil,nil]],

     [["'99/5/2",false],[99,5,2,nil,nil,nil,nil,nil,nil]],
     [["'99/5/2",true],[1999,5,2,nil,nil,nil,nil,nil,nil]],
    ].each do |x,y|
      h = Date._parse(*x)
      a = h.values_at(:year,:mon,:mday,:hour,:min,:sec,:zone,:offset,:wday)
      if y[1] == -1
	a[1] = -1
	a[2] = h[:yday]
      end
      assert_equal(y, a, [x, y, a].inspect)
    end
  end

  def test__parse__2
    h = Date._parse('22:45:59.5')
    assert_equal([22, 45, 59, 5.to_r/10**1], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('22:45:59.05')
    assert_equal([22, 45, 59, 5.to_r/10**2], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('22:45:59.005')
    assert_equal([22, 45, 59, 5.to_r/10**3], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('22:45:59.0123')
    assert_equal([22, 45, 59, 123.to_r/10**4], h.values_at(:hour, :min, :sec, :sec_fraction))

    h = Date._parse('224559.5')
    assert_equal([22, 45, 59, 5.to_r/10**1], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('224559.05')
    assert_equal([22, 45, 59, 5.to_r/10**2], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('224559.005')
    assert_equal([22, 45, 59, 5.to_r/10**3], h.values_at(:hour, :min, :sec, :sec_fraction))
    h = Date._parse('224559.0123')
    assert_equal([22, 45, 59, 123.to_r/10**4], h.values_at(:hour, :min, :sec, :sec_fraction))

    h = Date._parse('2006-w15-5')
    assert_equal([2006, 15, 5], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('2006w155')
    assert_equal([2006, 15, 5], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('06w155', false)
    assert_equal([6, 15, 5], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('06w155', true)
    assert_equal([2006, 15, 5], h.values_at(:cwyear, :cweek, :cwday))

    h = Date._parse('2006-w15')
    assert_equal([2006, 15, nil], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('2006w15')
    assert_equal([2006, 15, nil], h.values_at(:cwyear, :cweek, :cwday))

    h = Date._parse('-w15-5')
    assert_equal([nil, 15, 5], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('-w155')
    assert_equal([nil, 15, 5], h.values_at(:cwyear, :cweek, :cwday))

    h = Date._parse('-w15')
    assert_equal([nil, 15, nil], h.values_at(:cwyear, :cweek, :cwday))
    h = Date._parse('-w15')
    assert_equal([nil, 15, nil], h.values_at(:cwyear, :cweek, :cwday))

    h = Date._parse('-w-5')
    assert_equal([nil, nil, 5], h.values_at(:cwyear, :cweek, :cwday))

    h = Date._parse('--11-29')
    assert_equal([nil, 11, 29], h.values_at(:year, :mon, :mday))
    h = Date._parse('--1129')
    assert_equal([nil, 11, 29], h.values_at(:year, :mon, :mday))
    h = Date._parse('--11')
    assert_equal([nil, 11, nil], h.values_at(:year, :mon, :mday))
    h = Date._parse('---29')
    assert_equal([nil, nil, 29], h.values_at(:year, :mon, :mday))
    h = Date._parse('-333')
    assert_equal([nil, 333], h.values_at(:year, :yday))

    h = Date._parse('2006-333')
    assert_equal([2006, 333], h.values_at(:year, :yday))
    h = Date._parse('2006333')
    assert_equal([2006, 333], h.values_at(:year, :yday))
    h = Date._parse('06333', false)
    assert_equal([6, 333], h.values_at(:year, :yday))
    h = Date._parse('06333', true)
    assert_equal([2006, 333], h.values_at(:year, :yday))
    h = Date._parse('333')
    assert_equal([nil, 333], h.values_at(:year, :yday))
  end

  def test_parse
    assert_equal(Date.new, Date.parse)
    assert_equal(Date.new(2002,3,14), Date.parse('2002-03-14'))

    assert_equal(DateTime.new(2002,3,14,11,22,33, 0),
		 DateTime.parse('2002-03-14T11:22:33Z'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, 9.to_r/24),
		 DateTime.parse('2002-03-14T11:22:33+09:00'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, -9.to_r/24),
		 DateTime.parse('2002-03-14T11:22:33-09:00'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, -9.to_r/24) + 123456789.to_r/1000000000/86400,
		 DateTime.parse('2002-03-14T11:22:33.123456789-09:00'))
  end

  def test_parse__2
    d1 = DateTime.parse('2004-03-13T22:45:59.5')
    d2 = DateTime.parse('2004-03-13T22:45:59')
    assert_equal(d2 + 5.to_r/10**1/86400, d1)
    d1 = DateTime.parse('2004-03-13T22:45:59.05')
    d2 = DateTime.parse('2004-03-13T22:45:59')
    assert_equal(d2 + 5.to_r/10**2/86400, d1)
    d1 = DateTime.parse('2004-03-13T22:45:59.005')
    d2 = DateTime.parse('2004-03-13T22:45:59')
    assert_equal(d2 + 5.to_r/10**3/86400, d1)
    d1 = DateTime.parse('2004-03-13T22:45:59.0123')
    d2 = DateTime.parse('2004-03-13T22:45:59')
    assert_equal(d2 + 123.to_r/10**4/86400, d1)
    d1 = DateTime.parse('2004-03-13T22:45:59.5')
    d1 += 1.to_r/2/86400
    d2 = DateTime.parse('2004-03-13T22:46:00')
    assert_equal(d2, d1)
  end

  require 'time'

  def test_parse__time
    methods = [:to_s, :asctime, :iso8601, :rfc2822, :httpdate, :xmlschema]

    t = Time.utc(2001,2,3,4,5,6)
    methods.each do |m|
      d = DateTime.parse(t.__send__(m))
      assert_equal([2001, 2, 3, 4, 5, 6],
		   [d.year, d.mon, d.mday, d.hour, d.min, d.sec],
		   [m, t.__send__(m)].inspect)
    end

    t = Time.mktime(2001,2,3,4,5,6)
    methods.each do |m|
      next if m == :httpdate
      d = DateTime.parse(t.__send__(m))
      assert_equal([2001, 2, 3, 4, 5, 6],
		   [d.year, d.mon, d.mday, d.hour, d.min, d.sec],
		   [m, t.__send__(m)].inspect)
    end
  end

  def test_parse__comp
    n = DateTime.now

    d = DateTime.parse('073')
    assert_equal([n.year, 73, 0, 0, 0],
		 [d.year, d.yday, d.hour, d.min, d.sec])
    d = DateTime.parse('13')
    assert_equal([n.year, n.mon, 13, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.parse('Mar 13')
    assert_equal([n.year, 3, 13, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.parse('Mar 2004')
    assert_equal([2004, 3, 1, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.parse('23:55')
    assert_equal([n.year, n.mon, n.mday, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.parse('23:55:30')
    assert_equal([n.year, n.mon, n.mday, 23, 55, 30],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.parse('Sun 23:55')
    d2 = d - d.wday
    assert_equal([d2.year, d2.mon, d2.mday, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.parse('Aug 23:55')
    assert_equal([n.year, 8, 1, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
  end

  def test_parse__d_to_s
    d = Date.new(2002,3,14)
    assert_equal(d, Date.parse(d.to_s))

    d = DateTime.new(2002,3,14,11,22,33, 9.to_r/24)
    assert_equal(d, DateTime.parse(d.to_s))
  end

  def test_parse_utf8
    h = DateTime._parse("Sun\u{3000}Aug 16 01:02:03 \u{65e5}\u{672c} 2009")
    assert_equal(2009, h[:year])
    assert_equal(8, h[:mon])
    assert_equal(16, h[:mday])
    assert_equal(0, h[:wday])
    assert_equal(1, h[:hour])
    assert_equal(2, h[:min])
    assert_equal(3, h[:sec])
    assert_equal("\u{65e5}\u{672c}", h[:zone])
  end

  def test_parse__ex
    assert_raise(ArgumentError) do
      Date.parse('')
    end
    assert_raise(ArgumentError) do
      DateTime.parse('')
    end
    assert_raise(ArgumentError) do
      Date.parse('2001-02-29')
    end
    assert_raise(ArgumentError) do
      DateTime.parse('2001-02-29T23:59:60')
    end
    assert_raise(ArgumentError) do
      Date.parse('23:55')
    end
  end

  def test__iso8601
    h = Date._iso8601('01-02-03T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-02-03T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('--02-03T04:05:06Z')
    assert_equal([nil, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('---03T04:05:06Z')
    assert_equal([nil, nil, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('2001-02-03T04:05')
    assert_equal([2001, 2, 3, 4, 5, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-02-03T04:05:06')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-02-03T04:05:06,07')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-02-03T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-02-03T04:05:06.07+01:00')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('010203T040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('--0203T040506Z')
    assert_equal([nil, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('---03T040506Z')
    assert_equal([nil, nil, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('010203T0405')
    assert_equal([2001, 2, 3, 4, 5, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T0405')
    assert_equal([2001, 2, 3, 4, 5, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T040506')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T040506,07')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203T040506.07+0100')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('200102030405')
    assert_equal([2001, 2, 3, 4, 5, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203040506')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203040506,07')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('20010203040506.07+0100')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('01-023T04:05:06Z')
    assert_equal([2001, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-023T04:05:06Z')
    assert_equal([2001, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-023T04:05:06Z')
    assert_equal([nil, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))

    h = Date._iso8601('01023T040506Z')
    assert_equal([2001, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001023T040506Z')
    assert_equal([2001, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-023T040506Z')
    assert_equal([nil, 23, 4, 5, 6, 0],
		 h.values_at(:year, :yday, :hour, :min, :sec, :offset))

    h = Date._iso8601('01-w02-3T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001-w02-3T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-w02-3T04:05:06Z')
    assert_equal([nil, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-w-3T04:05:06Z')
    assert_equal([nil, nil, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))

    h = Date._iso8601('01w023T040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('2001w023T040506Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-w023T040506Z')
    assert_equal([nil, 2, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))
    h = Date._iso8601('-w-3T040506Z')
    assert_equal([nil, nil, 3, 4, 5, 6, 0],
		 h.values_at(:cwyear, :cweek, :cwday, :hour, :min, :sec, :offset))

    h = Date._iso8601('04:05')
    assert_equal([nil, nil, nil, 4, 5, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('04:05:06')
    assert_equal([nil, nil, nil, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('04:05:06,07')
    assert_equal([nil, nil, nil, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('04:05:06Z')
    assert_equal([nil, nil, nil, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('04:05:06.07+01:00')
    assert_equal([nil, nil, nil, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._iso8601('040506,07')
    assert_equal([nil, nil, nil, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._iso8601('040506.07+0100')
    assert_equal([nil, nil, nil, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

  end

  def test__rfc3339
    h = Date._rfc3339('2001-02-03T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc3339('2001-02-03 04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc3339('2001-02-03T04:05:06.07+01:00')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
  end

  def test__xmlschema
    h = Date._xmlschema('2001-02-03')
    assert_equal([2001, 2, 3, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-03Z')
    assert_equal([2001, 2, 3, nil, nil, nil, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-03+01:00')
    assert_equal([2001, 2, 3, nil, nil, nil, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('2001-02-03T04:05:06')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-03T04:05:06.07')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-03T04:05:06.07Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-03T04:05:06.07+01:00')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('04:05:06')
    assert_equal([nil, nil, nil, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('04:05:06Z')
    assert_equal([nil, nil, nil, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('04:05:06+01:00')
    assert_equal([nil, nil, nil, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('2001-02')
    assert_equal([2001, 2, nil, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02Z')
    assert_equal([2001, 2, nil, nil, nil, nil, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02+01:00')
    assert_equal([2001, 2, nil, nil, nil, nil, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-02-01:00')
    assert_equal([2001, 2, nil, nil, nil, nil, -3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('2001')
    assert_equal([2001, nil, nil, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001Z')
    assert_equal([2001, nil, nil, nil, nil, nil, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001+01:00')
    assert_equal([2001, nil, nil, nil, nil, nil, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('2001-01:00')
    assert_equal([2001, nil, nil, nil, nil, nil, -3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('--02')
    assert_equal([nil, 2, nil, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('--02Z')
    assert_equal([nil, 2, nil, nil, nil, nil, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._xmlschema('--02+01:00')
    assert_equal([nil, 2, nil, nil, nil, nil, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('92001-02-03T04:05:06.07+01:00')
    assert_equal([92001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._xmlschema('-92001-02-03T04:05:06.07+01:00')
    assert_equal([-92001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
  end

  def test__rfc2822
    h = Date._rfc2822('Sat, 3 Feb 2001 04:05:06 UT')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc2822('Sat, 3 Feb 2001 04:05:06 EST')
    assert_equal([2001, 2, 3, 4, 5, 6, -5*3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc2822('Sat, 3 Feb 2001 04:05:06 +0000')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc2822('Sat, 3 Feb 2001 04:05:06 +0100')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._rfc2822('Sat, 03 Feb 50 04:05:06 +0100')
    assert_equal([1950, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc2822('Sat, 03 Feb 49 04:05:06 +0100')
    assert_equal([2049, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._rfc2822('Sat, 03 Feb 100 04:05:06 +0100')
    assert_equal([2000, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h1 = Date._rfc2822('Sat, 3 Feb 2001 04:05:06 UT')
    h2 = Date._rfc822('Sat, 3 Feb 2001 04:05:06 UT')
    assert_equal(h1, h2)
  end

  def test__httpdate
    h = Date._httpdate('Sat, 03 Feb 2001 04:05:06 GMT')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._httpdate('Saturday, 03-Feb-01 04:05:06 GMT')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._httpdate('Sat Feb  3 04:05:06 2001')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._httpdate('Sat Feb 03 04:05:06 2001')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
  end

  def test__jisx0301
    h = Date._jisx0301('13.02.03')
    assert_equal([2001, 2, 3, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._jisx0301('H13.02.03')
    assert_equal([2001, 2, 3, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._jisx0301('S63.02.03')
    assert_equal([1988, 2, 3, nil, nil, nil, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))

    h = Date._jisx0301('H13.02.03T04:05:06')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._jisx0301('H13.02.03T04:05:06,07')
    assert_equal([2001, 2, 3, 4, 5, 6, nil],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._jisx0301('H13.02.03T04:05:06Z')
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
    h = Date._jisx0301('H13.02.03T04:05:06.07+0100')
    assert_equal([2001, 2, 3, 4, 5, 6, 3600],
		 h.values_at(:year, :mon, :mday, :hour, :min, :sec, :offset))
  end

  def test_iso8601
    assert_instance_of(Date, Date.iso8601)
    assert_instance_of(DateTime, DateTime.iso8601)

    d = Date.iso8601('2001-02-03', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.iso8601('2001-02-03T04:05:06+07:00', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+07:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_rfc3339
    assert_instance_of(Date, Date.rfc3339)
    assert_instance_of(DateTime, DateTime.rfc3339)

    d = Date.rfc3339('2001-02-03T04:05:06+07:00', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.rfc3339('2001-02-03T04:05:06+07:00', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+07:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_xmlschema
    assert_instance_of(Date, Date.xmlschema)
    assert_instance_of(DateTime, DateTime.xmlschema)

    d = Date.xmlschema('2001-02-03', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.xmlschema('2001-02-03T04:05:06+07:00', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+07:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_rfc2822
    assert_instance_of(Date, Date.rfc2822)
    assert_instance_of(DateTime, DateTime.rfc2822)
    assert_instance_of(Date, Date.rfc822)
    assert_instance_of(DateTime, DateTime.rfc822)

    d = Date.rfc2822('Sat, 3 Feb 2001 04:05:06 +0700', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.rfc2822('Sat, 3 Feb 2001 04:05:06 +0700', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+07:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_httpdate
    assert_instance_of(Date, Date.httpdate)
    assert_instance_of(DateTime, DateTime.httpdate)

    d = Date.httpdate('Sat, 03 Feb 2001 04:05:06 GMT', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.httpdate('Sat, 03 Feb 2001 04:05:06 GMT', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+00:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_jisx0301
    assert_instance_of(Date, Date.jisx0301)
    assert_instance_of(DateTime, DateTime.jisx0301)

    d = Date.jisx0301('H13.02.03', Date::ITALY + 10)
    assert_equal(Date.new(2001,2,3), d)
    assert_equal(Date::ITALY + 10, d.start)

    d = DateTime.jisx0301('H13.02.03T04:05:06+07:00', Date::ITALY + 10)
    assert_equal(DateTime.new(2001,2,3,4,5,6,'+07:00'), d)
    assert_equal(Date::ITALY + 10, d.start)
  end

  def test_given_string
    s = '2001-02-03T04:05:06Z'
    s0 = s.dup

    assert_not_equal({}, Date._parse(s))
    assert_equal(s0, s)

    assert_not_equal({}, Date._iso8601(s))
    assert_equal(s0, s)

    assert_not_equal({}, Date._rfc3339(s))
    assert_equal(s0, s)

    assert_not_equal({}, Date._xmlschema(s))
    assert_equal(s0, s)

    s = 'Sat, 3 Feb 2001 04:05:06 UT'
    s0 = s.dup
    assert_not_equal({}, Date._rfc2822(s))
    assert_equal(s0, s)
    assert_not_equal({}, Date._rfc822(s))
    assert_equal(s0, s)

    s = 'Sat, 03 Feb 2001 04:05:06 GMT'
    s0 = s.dup
    assert_not_equal({}, Date._httpdate(s))
    assert_equal(s0, s)

    s = 'H13.02.03T04:05:06,07Z'
    s0 = s.dup
    assert_not_equal({}, Date._jisx0301(s))
    assert_equal(s0, s)
  end

end
