;; -*-Emacs-Lisp-*-
;;
;; ruby-electric.el --- electric editing commands for ruby files
;;
;; Copyright (C) 2005 by Dee Zsombor <dee dot zsombor at gmail dot com>.
;; Released under same license terms as Ruby.
;;
;; Due credit: this work was inspired by a code snippet posted by
;; Frederick Ros at http://rubygarden.org/ruby?EmacsExtensions.
;;
;; Following improvements where added:
;;
;;       - handling of strings of type 'here document'
;;       - more keywords, with special handling for 'do'
;;       - packaged into a minor mode
;;
;; Usage:
;;
;;    0) copy ruby-electric.el into directory where emacs can find it.
;;
;;    1) modify your startup file (.emacs or whatever) by adding
;;       following line:
;;
;;            (require 'ruby-electric)
;;
;;       note that you need to have font lock enabled beforehand.
;;
;;    2) toggle Ruby Electric Mode on/off with ruby-electric-mode.
;;
;; Changelog:
;;
;;  2005/Jan/14: inserts matching pair delimiters like {, [, (, ', ",
;;  ' and | .
;;
;;  2005/Jan/14: added basic Custom support for configuring keywords
;;  with electric closing.
;;
;;  2005/Jan/18: more Custom support for configuring characters for
;;  which matching expansion should occur.
;;
;;  2005/Jan/18: no longer uses 'looking-back' or regexp character
;;  classes like [:space:] since they are not implemented on XEmacs.
;;
;;  2005/Feb/01: explicitly provide default argument of 1 to
;;  'backward-word' as it requires it on Emacs 21.3
;;
;;  2005/Mar/06: now stored inside ruby CVS; customize pages now have
;;  ruby as parent; cosmetic fixes.


(require 'ruby-mode)

(defgroup ruby-electric nil
  "Minor mode providing electric editing commands for ruby files"
  :group 'ruby) 

(defconst ruby-electric-expandable-do-re
  "do\\s-$")

(defconst ruby-electric-expandable-bar
  "\\s-\\(do\\|{\\)\\s-+|")

(defvar ruby-electric-matching-delimeter-alist
  '((?\[ . ?\])
    (?\( . ?\))
    (?\' . ?\')
    (?\` . ?\`)
    (?\" . ?\")))

(defcustom ruby-electric-simple-keywords-re
  (regexp-opt '("def" "if" "class" "module" "unless" "case" "while" "do" "until" "for" "begin") t)
  "*Regular expresion matching keywords for which closing 'end'
is to be inserted."
  :type 'regexp :group 'ruby-electric)

(defcustom ruby-electric-expand-delimiters-list '(all)
  "*List of contexts where matching delimiter should be
inserted. The word 'all' will do all insertions."
  :type '(set :extra-offset 8
              (const :tag "Everything" all )
              (const :tag "Curly brace" ?\{ )
              (const :tag "Square brace" ?\[ )
              (const :tag "Round brace" ?\( )
              (const :tag "Quote" ?\' )
              (const :tag "Double quote" ?\" )
              (const :tag "Back quote" ?\` )
              (const :tag "Vertical bar" ?\| ))
  :group 'ruby-electric)

(defcustom ruby-electric-newline-before-closing-bracket nil
  "*Controls whether a newline should be inserted before the
closing bracket or not."
  :type 'boolean :group 'ruby-electric)

(define-minor-mode ruby-electric-mode
  "Toggle Ruby Electric minor mode.
With no argument, this command toggles the mode.  Non-null prefix
argument turns on the mode.  Null prefix argument turns off the
mode.

When Ruby Electric mode is enabled, an indented 'end' is
heuristicaly inserted whenever typing a word like 'module',
'class', 'def', 'if', 'unless', 'case', 'until', 'for', 'begin',
'do'. Simple, double and back quotes as well as braces are paired
auto-magically. Expansion does not occur inside comments and
strings. Note that you must have Font Lock enabled."
  ;; initial value.
  nil
  ;;indicator for the mode line.
  " REl"
  ;;keymap
  ruby-mode-map
  (ruby-electric-setup-keymap))

(defun ruby-electric-setup-keymap()
  (define-key ruby-mode-map " " 'ruby-electric-space)
  (define-key ruby-mode-map "{" 'ruby-electric-curlies)
  (define-key ruby-mode-map "(" 'ruby-electric-matching-char)
  (define-key ruby-mode-map "[" 'ruby-electric-matching-char)
  (define-key ruby-mode-map "\"" 'ruby-electric-matching-char)
  (define-key ruby-mode-map "\'" 'ruby-electric-matching-char)
  (define-key ruby-mode-map "|" 'ruby-electric-bar))

(defun ruby-electric-space (arg)
  (interactive "P")
  (self-insert-command (prefix-numeric-value arg))
  (if (ruby-electric-space-can-be-expanded-p)
      (save-excursion
        (ruby-indent-line t)
        (newline)
        (ruby-insert-end))))

(defun ruby-electric-code-at-point-p()
  (and ruby-electric-mode
       (let* ((properties (text-properties-at (point))))
         (and (null (memq 'font-lock-string-face properties))
              (null (memq 'font-lock-comment-face properties))))))

(defun ruby-electric-string-at-point-p()
  (and ruby-electric-mode
       (consp (memq 'font-lock-string-face (text-properties-at (point))))))

(defun ruby-electric-is-last-command-char-expandable-punct-p()
  (or (memq 'all ruby-electric-expand-delimiters-list)
      (memq last-command-char ruby-electric-expand-delimiters-list)))

(defun ruby-electric-space-can-be-expanded-p()
  (if (ruby-electric-code-at-point-p)
      (let* ((ruby-electric-keywords-re
              (concat ruby-electric-simple-keywords-re "\\s-$"))
             (ruby-electric-single-keyword-in-line-re
              (concat "\\s-*" ruby-electric-keywords-re)))
        (save-excursion
          (backward-word 1)
          (or (looking-at ruby-electric-expandable-do-re)
              (and (looking-at ruby-electric-keywords-re)
                   (not (string= "do" (match-string 1)))
                   (progn
                     (beginning-of-line)
                     (looking-at ruby-electric-single-keyword-in-line-re))))))))


(defun ruby-electric-curlies(arg)
  (interactive "P")
  (self-insert-command (prefix-numeric-value arg))
  (if (ruby-electric-is-last-command-char-expandable-punct-p)
      (cond ((ruby-electric-code-at-point-p)
             (insert " ")
             (save-excursion
               (if ruby-electric-newline-before-closing-bracket
                   (newline))
               (insert "}")))
            ((ruby-electric-string-at-point-p)
             (if (eq last-command-event ?{)
                 (save-excursion
                   (when (not (char-equal ?\# (preceding-char)))
                       (delete-backward-char)
                       (insert "#"))))
             (save-excursion
               (backward-char 1)
               (when (char-equal ?\# (preceding-char))
                 (forward-char 1)
                 (insert "}")))))))

(defun ruby-electric-matching-char(arg)
  (interactive "P")
  (self-insert-command (prefix-numeric-value arg))
  (and (ruby-electric-is-last-command-char-expandable-punct-p)
       (ruby-electric-code-at-point-p)
       (save-excursion
         (insert (cdr (assoc last-command-char
                             ruby-electric-matching-delimeter-alist))))))

(defun ruby-electric-bar(arg)
  (interactive "P")
  (self-insert-command (prefix-numeric-value arg))
  (and (ruby-electric-is-last-command-char-expandable-punct-p)
       (ruby-electric-code-at-point-p)
       (and (save-excursion (re-search-backward ruby-electric-expandable-bar nil t))
            (= (point) (match-end 0))) ;looking-back is missing on XEmacs
       (save-excursion
         (insert "|"))))


(provide 'ruby-electric)
