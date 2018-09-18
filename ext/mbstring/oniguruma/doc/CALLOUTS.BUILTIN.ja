CALLOUTS.BUILTIN.ja               2018/03/26


* FAIL    (前進)

  (*FAIL)

  常に失敗する


* MISMATCH    (前進)

  (*MISMATCH)

  照合を中止する
  検索は継続する


* ERROR    (前進)

  (*ERROR{n::LONG})

  検索/照合を中止する
  戻り値は引数'n'の値。(-1より小さい負の値でなければならない)
  'n'はオプション引数で、デフォルト値はONIG_ABORT


* MAX    (前進/後退)

  (*MAX{n::LONG/TAG, c::CHAR})

  成功(デフォルト)、前進または後退回数を制限する
  'n'がTAGのときは、そのTAGのcalloutのslot 0の値が使用される
  'c'引数の値によって、slot 0の値が変化する
  'c'はオプション引数で、デフォルト値は'X'

  例:  "(?:(*COUNT[T]{X})a)*(?:(*MAX{T})c)*"

  [callout data]
  slot 0: '>': 前進回数, '<': 後退回数, 'X': 成功回数(デフォルト)


* COUNT    (前進/後退)

  (*COUNT{c::CHAR})

  カウンタ
  'c'引数の値によって、slot 0の値が変化する
  'c'はオプション引数で、デフォルト値は'>'

  [callout data]
  slot 0: '>': 前進回数(デフォルト), '<': 後退回数, 'X': 成功回数
  slot 1: 前進回数
  slot 2: 後退回数

  (* 成功回数 = 前進回数 - 後退回数)

  ** ONIG_OPTION_FIND_LONGEST または ONIG_OPTION_FIND_NOT_EMPTY が使用されると
     正確な動作ができなくなる


* TOTAL_COUNT    (前進/後退)

  (*TOTAL_COUNT{c::CHAR})

  これはCOUNTとほとんど同じ
  しかし、カウントが検索過程で積算される
  'c'はオプション引数で、デフォルト値は'>'

  [callout data]
  slot 0: '>': 前進回数(デフォルト), '<': 後退回数, 'X': 成功回数
  slot 1: 前進回数
  slot 2: 後退回数

  ** ONIG_OPTION_FIND_LONGEST または ONIG_OPTION_FIND_NOT_EMPTY が使用されると
     正確な動作ができなくなる


* CMP    (前進)

  (*CMP{x::TAG/LONG, op::STRING, y::TAG/LONG})

  xの値とyの値をop演算子で比較する
  x, yがTAGのときにはそのcalloutのslot 0の値が参照される

  op: '==', '!=', '>', '<', '>=', '<='

  例: "(?:(*MAX[TA]{7})a|(*MAX[TB]{5})b)*(*CMP{TA,>=,4})"

  [callout data]
  slot 0: op値 (src/regexec.c の中の enum OP_CMP)

//END
