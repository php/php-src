# -*- coding: utf-8 -*-
#   irb/lc/ja/error.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require "e2mmap"

module IRB
  # exceptions
  extend Exception2MessageMapper
  def_exception :UnrecognizedSwitch, 'スイッチ(%s)が分りません'
  def_exception :NotImplementedError, '`%s\'の定義が必要です'
  def_exception :CantReturnToNormalMode, 'Normalモードに戻れません.'
  def_exception :IllegalParameter, 'パラメータ(%s)が間違っています.'
  def_exception :IrbAlreadyDead, 'Irbは既に死んでいます.'
  def_exception :IrbSwitchedToCurrentThread, 'カレントスレッドに切り替わりました.'
  def_exception :NoSuchJob, 'そのようなジョブ(%s)はありません.'
  def_exception :CantShiftToMultiIrbMode, 'multi-irb modeに移れません.'
  def_exception :CantChangeBinding, 'バインディング(%s)に変更できません.'
  def_exception :UndefinedPromptMode, 'プロンプトモード(%s)は定義されていません.'
end
# vim:fileencoding=utf-8
