Callouts API  Version 6.9.7 2022/02/04

#include <oniguruma.h>

(1) 呼び出し関数
(2) 内容の呼び出し関数の設定/取得
(3) 名前の呼び出し関数の設定
(4) ユーザデータ
(5) OnigCalloutArgsからの値の取得
(6) 名札
(7) 呼び出しデータ (呼び出し関数内から使用される)
(8) 呼び出しデータ (アプリケーションから使用される)
(9) その他の関数


(1) 呼び出し関数

  型: OnigCalloutFunc

  typedef int (*OnigCalloutFunc)(OnigCalloutArgs* args, void* user_data);

  若し呼び出し関数として０(NULL)がセットされると、呼ばれることはない


  * 呼び出し関数の戻り値 (int)

    ONIG_CALLOUT_FAIL (== 1):     失敗
    ONIG_CALLOUT_SUCCESS (== 0):  成功
    -1未満:                       エラーコード (検索/照合の終了)

    ONIG_CALLOUT_FAIL/SUCCESSは、後退中の呼び出しでは無視される。
    後退は失敗の回復過程なので。

  * 呼び出し関数の例

    extern int always_success(OnigCalloutArgs* args, void* user_data)
    {
      return ONIG_CALLOUT_SUCCESS;
    }



(2) 内容の呼び出し関数の設定/取得

# OnigCalloutFunc onig_get_progress_callout(void)

  内容の呼び出し関数(前進中)を返す


# int onig_set_progress_callout(OnigCalloutFunc f)

  内容の呼び出し関数(前進中)をセットする。
  この値はonig_initialize_match_param()の中でデフォルトの呼び出し関数として
  セットされる。

  正常終了: ONIG_NORMAL


# OnigCalloutFunc onig_get_retraction_callout(void)

  内容の呼び出し関数(後退中)を返す


# int onig_set_retraction_callout(OnigCalloutFunc f)

  内容の呼び出し関数(後退中)をセットする。
  この値はonig_initialize_match_param()の中でデフォルトの呼び出し関数として
  セットされる。

  正常終了: ONIG_NORMAL


# int onig_set_progress_callout_of_match_param(OnigMatchParam* mp, OnigCalloutFunc f)

  内容の呼び出し関数(前進中)をセットする。

  引数
  1 mp: match-paramアドレス
  2 f: 関数

  正常終了: ONIG_NORMAL


# int onig_set_retraction_callout_of_match_param(OnigMatchParam* mp, OnigCalloutFunc f)

  内容の呼び出し関数(後退中)をセットする。

  引数
  1 mp: match-paramアドレス
  2 f: 関数

  正常終了: ONIG_NORMAL



(3) 名前の呼び出し関数の設定

# int onig_set_callout_of_name(OnigEncoding enc, OnigCalloutType type, OnigUChar* name, OnigUChar* name_end, int callout_in, OnigCalloutFunc callout, OnigCalloutFunc end_callout, int arg_num, unsigned int arg_types[], int opt_arg_num, OnigValue opt_defaults[])

  名前の呼び出し関数をセットする。
  名前に許される文字: _ A-Z a-z 0-9   (* 最初の文字: _ A-Z a-z)

  (enc, name)のペアが、呼び出し関数を見つけるためのキーとして使用される。
  アプリケーションで使用される各エンコーディングに対してこの関数を呼ぶ必要がある。
  しかし若しencエンコーディングがASCII互換であり、(enc, name)に対するエントリが
  見つからない場合には、(ASCII, name)エントリが参照される。
  従って、若しASCII互換エンコーディングのみ使用している場合には、この関数を(ASCII, name)
  について一回呼べば十分である。

  引数
   1 enc:         文字エンコーディング
   2 type:        呼び出し型 (現在は ONIG_CALLOUT_TYPE_SINGLE のみサポート)
   3 name:        名前のアドレス (encでエンコーディングされている文字列)
   4 name_end:    名前の終端アドレス
   5 callout_in:  方向フラグ (ONIG_CALLOUT_IN_PROGRESS/RETRACTION/BOTH)
   6 callout:     呼び出し関数
   7 end_callout: *まだ使用していない (０をセット)
   8 arg_num:     引数の数 (* 最大値 ONIG_CALLOUT_MAX_ARGS_NUM == 4)
   9 arg_types:   引数の型の配列
  10 opt_arg_num: オプション引数の数
  11 opt_defaults: オプション引数のデフォルト値

  正常終了: ONIG_NORMAL
  error:
    ONIGERR_INVALID_CALLOUT_NAME
    ONIGERR_INVALID_ARGUMENT
    ONIGERR_INVALID_CALLOUT_ARG



(4) ユーザデータ

# int onig_set_callout_user_data_of_match_param(OnigMatchParam* param, void* user_data)

  呼び出し関数の引数として渡されるユーザデータをセットする。

  正常終了: ONIG_NORMAL



(5) OnigCalloutArgsからの値の取得

# int onig_get_callout_num_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの呼び出し番号を返す。
  "呼び出し番号"とは、正規表現パターンの中の呼び出しに対する識別子である。


# OnigCalloutIn onig_get_callout_in_by_callout_args(OnigCalloutArgs* args)

  この呼び出しが起きた時の方向(前進中/後退中)を返す。
  (ONIG_CALLOUT_IN_PROGRESS か ONIG_CALLOUT_IN_RETRACTION)


# int onig_get_name_id_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの名前(name)の識別子を返す。
  若しこの呼び出しが内容の呼び出しのときには、ONIG_NON_NAME_IDが返される。


# const OnigUChar* onig_get_contents_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの内容文字列(NULL終端あり)を返す。
  若しこの呼び出しが名前の呼び出しのときには、NULLを返す。


# const OnigUChar* onig_get_contents_end_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの内容(contents)の終端を返す。
  若しこの呼び出しが名前の呼び出しのときには、NULLを返す。


# int onig_get_args_num_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの引数の数を返す。
  正規表現パターンの中で渡されなかったオプション引数も含む。
  若しこの呼び出しが内容の呼び出しのときには、ONIGERR_INVALID_ARGUMENTが返される。


# int onig_get_passed_args_num_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの本当に渡された引数の数を返す。
  若しこの呼び出しが内容の呼び出しのときには、ONIGERR_INVALID_ARGUMENTが返される。


# int onig_get_arg_by_callout_args(OnigCalloutArgs* args, int index, OnigType* type, OnigValue* val)

  この呼び出しの一個の引数の値と型を返す。
  若しこの呼び出しが内容の呼び出しのときには、ONIGERR_INVALID_ARGUMENTが返される。

  正常終了: ONIG_NORMAL


# const OnigUChar* onig_get_string_by_callout_args(OnigCalloutArgs* args)

  対象文字列のアドレスを返す。
  onig_search()の二番目の引数(str)である。


# const OnigUChar* onig_get_string_end_by_callout_args(OnigCalloutArgs* args)

  対象文字列の終端アドレスを返す。
  onig_search()の三番目の引数(end)である。


# const OnigUChar* onig_get_start_by_callout_args(OnigCalloutArgs* args)

  対象文字列の現在の照合処理開始アドレスを返す。


# const OnigUChar* onig_get_right_range_by_callout_args(OnigCalloutArgs* args)

  対象文字列の現在の照合範囲アドレスを返す。


# const OnigUChar* onig_get_current_by_callout_args(OnigCalloutArgs* args)

  対象文字列の現在の照合位置アドレスを返す。


# OnigRegex onig_get_regex_by_callout_args(OnigCalloutArgs* args)

  この呼び出しの正規表現オブジェクトのアドレスを返す。


# unsigned long onig_get_retry_counter_by_callout_args(OnigCalloutArgs* args)

  retry-limit-in-matchのためのリトライカウンタの現在値を返す。



(6) 名札

  "Tag" (名札)とは、正規表現パターンの中で呼び出しに割り当てられた名前である。
  tag文字列に使用できる文字: _ A-Z a-z 0-9   (* 先頭の文字: _ A-Z a-z)


# int onig_callout_tag_is_exist_at_callout_num(OnigRegex reg, int callout_num)

  その呼び出しにtagが割り当てられていれば1を返す、そうでなければ0を返す。


# const OnigUChar* onig_get_callout_tag_start(OnigRegex reg, int callout_num)

  その呼び出しに対するtag文字列(NULL終端あり)の先頭アドレスを返す。


# const OnigUChar* onig_get_callout_tag_end(OnigRegex reg, int callout_num)

  その呼び出しに対するtag文字列の終端アドレスを返す。


# int onig_get_callout_num_by_tag(OnigRegex reg, const OnigUChar* tag, const OnigUChar* tag_end)

  そのtagに対する呼び出し番号を返す。



(7) 呼び出しデータ (呼び出し関数内から使用される)

  "呼び出しデータ" (callout data)とは、
  それぞれの呼び出しに対してそれぞれの検索処理の中で割り当てられた、
  ONIG_CALLOUT_DATA_SLOT_NUM(== 5)個の値の領域である。
  一個の呼び出しに対する各値の領域は、"スロット"(slot)番号(0 - 4)によって示される。
  呼び出しデータは呼び出し関数の実装者によって任意の目的に使用される。


# int onig_get_callout_data_by_callout_args(OnigCalloutArgs* args, int callout_num, int slot, OnigType* type, OnigValue* val)

  callout_num/slotによって示された呼び出しスロットに対するデータの値/型を返す。

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


# int onig_get_callout_data_by_callout_args_self(OnigCalloutArgs* args, int slot, OnigType* type, OnigValue* val)

  自分自身の呼び出しのslotによって示されたスロットに対するデータの値/型を返す。

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


# int onig_set_callout_data_by_callout_args(OnigCalloutArgs* args, int callout_num, int slot, OnigType type, OnigValue* val)

  callout_num/slotによって示された呼び出しスロットに対する値/型をセットする。

  ONIG_NORMAL: 正常終了
          < 0: エラーコード


# int onig_set_callout_data_by_callout_args_self(OnigCalloutArgs* args, int slot, OnigType type, OnigValue* val)

  自分自身の呼び出しのslotによって示されたスロットに対する値/型をセットする。

  ONIG_NORMAL: 正常終了
          < 0: エラーコード


# int onig_get_callout_data_by_callout_args_self_dont_clear_old(OnigCalloutArgs* args, int slot, OnigType* type, OnigValue* val)

  この関数は、onig_get_callout_data_by_callout_args_self()とほぼ同じである。
  しかしこの関数は、現在の位置より以前の照合位置の中でセットされた値をクリアしない。(dont_clear_old)
  他のonig_get_callout_data_xxxx()関数は、以前の位置の照合処理の中でセットされた値をクリアする。

  例えば、組み込み呼び出し(*TOTAL_COUNT)は、検索処理の中の全ての照合処理の積算カウントを
  得るためにこの関数を使用して実装されている。
  組み込む呼び出し(*COUNT)は、この関数を使用しないので、最後の成功した照合処理だけの
  カウントを返す。

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


(8) 呼び出しデータ (アプリケーションから使用される)

# int onig_get_callout_data(OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType* type, OnigValue* val)

  callout_num/slotによって示された呼び出しスロットに対するデータの値/型を返す。

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


# int onig_get_callout_data_by_tag(OnigRegex reg, OnigMatchParam* mp, const OnigUChar* tag, const OnigUChar* tag_end, int slot, OnigType* type, OnigValue* val)

  tag/slotによって示された呼び出しスロットに対するデータの値/型を返す。

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


# int onig_set_callout_data(OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType type, OnigValue* val)

  callout_num/slotによって示された呼び出しスロットに対する値/型をセットする。

  ONIG_NORMAL: 正常終了
          < 0: エラーコード


# int onig_set_callout_data_by_tag(OnigRegex reg, OnigMatchParam* mp, const OnigUChar* tag, const OnigUChar* tag_end, int slot, OnigType type, OnigValue* val)

  tag/slotによって示された呼び出しスロットに対する値/型をセットする。

  ONIG_NORMAL: 正常終了
          < 0: エラーコード


# int onig_get_callout_data_dont_clear_old(OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType* type, OnigValue* val)

  この関数を使用する必要はないと思われる。
  廃止予定。


# int onig_get_callout_data_by_tag_dont_clear_old(regex_t* reg, OnigMatchParam* mp, const UChar* tag, const UChar* tag_end, int slot, OnigType* type, OnigValue* val)

  tag/slotによって示された呼び出しスロットに対するデータの値/型を返す。
  この関数は、現在の位置より以前の照合位置の中でセットされた値をクリアしない。
  (dont_clear_old)

  ONIG_NORMAL:           正常終了
  ONIG_VALUE_IS_NOT_SET: 値が未セット / 型がVOID
  < 0: エラーコード


(9) その他の関数

# OnigUChar* onig_get_callout_name_by_name_id(int name_id)

  名前の識別子に対する名前を返す。
  不正な識別子が渡された場合には０を返す。


# int onig_get_capture_range_in_callout(OnigCalloutArgs* args, int mem_num, int* begin, int* end)

  現在の捕獲範囲を返す。
  位置は、対象文字列に対するバイト単位で表される。
  未捕獲のmem_numに対しては、ONIG_REGION_NOTPOSがセットされる。


# int onig_get_used_stack_size_in_callout(OnigCalloutArgs* args, int* used_num, int* used_bytes)

  現在使用されている照合処理用スタックサイズを返す。

  used_num:   要素数
  used_bytes: バイト数

//END
