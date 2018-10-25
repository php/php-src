README.ja  2017/08/25

鬼車  ----   (C) K.Kosako <kkosako0@gmail.com>

https://github.com/kkos/oniguruma

鬼車は正規表現ライブラリである。
このライブラリの特長は、それぞれの正規表現オブジェクトごとに
文字エンコーディングを指定できることである。

サポートしている文字エンコーディング:

  ASCII, UTF-8, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE,
  EUC-JP, EUC-TW, EUC-KR, EUC-CN,
  Shift_JIS, Big5, GB18030, KOI8-R, CP1251,
  ISO-8859-1, ISO-8859-2, ISO-8859-3, ISO-8859-4, ISO-8859-5,
  ISO-8859-6, ISO-8859-7, ISO-8859-8, ISO-8859-9, ISO-8859-10,
  ISO-8859-11, ISO-8859-13, ISO-8859-14, ISO-8859-15, ISO-8859-16

* GB18030: 久保健洋氏提供
* CP1251:  Byte氏提供
------------------------------------------------------------

ライセンス

  BSDライセンス


インストール

 ケース１: UnixとCygwin環境

   1. autoreconf -vfi   (* configureスクリプトがないときだけ)

   2. ./configure
   3. make
   4. make install

   アンインストール

     make uninstall

   構成確認

     onig-config --cflags
     onig-config --libs
     onig-config --prefix
     onig-config --exec-prefix



 ケース２: Windows 64/32bit (Visual Studio)環境

   make_win64 あるいは make_win32 を実行

      onig_s.lib:  static link library
      onig.dll:    dynamic link library

   * 動作テスト (ASCII/Shift_JIS)
      1. cd src
      2. copy ..\windows\testc.c .
      3. nmake -f Makefile.windows ctest

   (Visual Studio Community 2015 で動作確認)



正規表現

  doc/RE.jaを参照


使用方法

  使用するプログラムで、oniguruma.hをインクルードする(Oniguruma APIの場合)。
  Oniguruma APIについては、doc/API.jaを参照。

  oniguruma.hで定義されている型名UChar(== unsigned char)を無効にしたい場合
  には、ONIG_ESCAPE_UCHAR_COLLISIONをdefineしてからoniguruma.hをインクルード
  すること。このときにはUCharは定義されず、OnigUCharという名前の定義のみが
  有効になる。

  oniguruma.hで定義されている型名regex_tを無効にしたい場合には、
  ONIG_ESCAPE_REGEX_T_COLLISIONをdefineしてからoniguruma.hをインクルード
  すること。このときにはregex_tは定義されず、OnigRegexType, OnigRegexという
  名前の定義のみが有効になる。

  Unix/Cygwin上でコンパイル、リンクする場合の例：
  (prefixが/usr/localのとき)
  cc sample.c -L/usr/local/lib -lonig

  GNU libtoolを使用しているので、プラットフォームが共有ライブラリをサポートして
  いれば、使用できるようになっている。
  静的ライブラリと共有ライブラリのどちらを使用するかを指定する方法、実行時点での
  環境設定方法については、自分で調べて下さい。


  Win32でスタティックリンクライブラリ(onig_s.lib)をリンクする場合には、
  コンパイルするときに -DONIG_EXTERN=extern をコンパイル引数に追加すること。


使用例プログラム

  sample/simple.c    最小例 (Oniguruma API)
  sample/names.c     名前付きグループコールバック使用例
  sample/encode.c    幾つかの文字エンコーディング使用例
  sample/listcap.c   捕獲履歴機能の使用例
  sample/posix.c     POSIX API使用例
  sample/sql.c       可変メタ文字機能使用例 (SQL-like パターン)
  sample/user_property.c ユーザ定義Unicodeプロパティの使用例


テストプログラム
  sample/syntax.c    Perl、Java、ASIS文法のテスト
  sample/crnl.c      --enable-crnl-as-line-terminator テスト


ソースファイル

  oniguruma.h        鬼車APIヘッダ (公開)
  onig-config.in     onig-configプログラム テンプレート

  regenc.h           文字エンコーディング枠組みヘッダ
  regint.h           内部宣言
  regparse.h         regparse.cとregcomp.cのための内部宣言
  regcomp.c          コンパイル、最適化関数
  regenc.c           文字エンコーディング枠組み
  regerror.c         エラーメッセージ関数
  regext.c           拡張API関数
  regexec.c          検索、照合関数
  regparse.c         正規表現パターン解析関数
  regsyntax.c        正規表現パターン文法関数、組込み文法定義
  regtrav.c          捕獲履歴木巡回関数
  regversion.c       版情報関数
  st.h               ハッシュテーブル関数宣言
  st.c               ハッシュテーブル関数

  oniggnu.h          GNU regex APIヘッダ (公開)
  reggnu.c           GNU regex API関数

  onigposix.h        POSIX APIヘッダ (公開)
  regposerr.c        POSIX APIエラーメッセージ関数
  regposix.c         POSIX API関数

  mktable.c          文字タイプテーブル生成プログラム
  ascii.c            ASCII エンコーディング
  euc_jp.c           EUC-JP エンコーディング
  euc_tw.c           EUC-TW エンコーディング
  euc_kr.c           EUC-KR, EUC-CN エンコーディング
  sjis.c             Shift_JIS エンコーディング
  big5.c             Big5 エンコーディング
  gb18030.c          GB18030 エンコーディング
  koi8.c             KOI8 エンコーディング
  koi8_r.c           KOI8-R エンコーディング
  cp1251.c           CP1251 エンコーディング
  iso8859_1.c        ISO-8859-1  (Latin-1)
  iso8859_2.c        ISO-8859-2  (Latin-2)
  iso8859_3.c        ISO-8859-3  (Latin-3)
  iso8859_4.c        ISO-8859-4  (Latin-4)
  iso8859_5.c        ISO-8859-5  (Cyrillic)
  iso8859_6.c        ISO-8859-6  (Arabic)
  iso8859_7.c        ISO-8859-7  (Greek)
  iso8859_8.c        ISO-8859-8  (Hebrew)
  iso8859_9.c        ISO-8859-9  (Latin-5 または Turkish)
  iso8859_10.c       ISO-8859-10 (Latin-6 または Nordic)
  iso8859_11.c       ISO-8859-11 (Thai)
  iso8859_13.c       ISO-8859-13 (Latin-7 または Baltic Rim)
  iso8859_14.c       ISO-8859-14 (Latin-8 または Celtic)
  iso8859_15.c       ISO-8859-15 (Latin-9 または West European with Euro)
  iso8859_16.c       ISO-8859-16
                     (Latin-10 または South-Eastern European with Euro)
  utf8.c             UTF-8    エンコーディング
  utf16_be.c         UTF-16BE エンコーディング
  utf16_le.c         UTF-16LE エンコーディング
  utf32_be.c         UTF-32BE エンコーディング
  utf32_le.c         UTF-32LE エンコーディング
  unicode.c          Unicodeエンコーディングの共通処理

  win32/Makefile     Win32用 Makefile (for VC++)
  win32/config.h     Win32用 config.h


and I'm thankful to Akinori MUSHA.
