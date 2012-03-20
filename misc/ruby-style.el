;;; -*- emacs-lisp -*-
;;;
;;; ruby-style.el -
;;;
;;; C/C++ mode style for Ruby.
;;;
;;;  $Author$
;;;  created at: Thu Apr 26 13:54:01 JST 2007
;;;
;;; To switch to the "ruby" style automatically if it looks like a
;;; source file of ruby, add ruby-style-c-mode to c-mode-hook:
;;;
;;;   (require 'ruby-style)
;;;   (add-hook 'c-mode-hook 'ruby-style-c-mode)
;;;   (add-hook 'c++-mode-hook 'ruby-style-c-mode)
;;;
;;; Customize the c-default-style variable to set the default style
;;; for each CC major mode.

(defconst ruby-style-revision "$Revision$"
  "Ruby style revision string.")

(defconst ruby-style-version
  (and
   (string-match "[0-9.]+" ruby-style-revision)
   (substring ruby-style-revision (match-beginning 0) (match-end 0)))
  "Ruby style version number.")

(defun ruby-style-case-indent (x)
  (save-excursion
    (back-to-indentation)
    (unless (progn (backward-up-list) (back-to-indentation)
                   (> (point) (cdr x)))
      (goto-char (cdr x))
      (if (looking-at "\\<case\\|default\\>") '*))))

(defun ruby-style-label-indent (x)
  (save-excursion
    (back-to-indentation)
    (unless (progn (backward-up-list) (back-to-indentation)
                   (>= (point) (cdr x)))
      (goto-char (cdr x))
      (condition-case ()
          (progn
            (backward-up-list)
            (backward-sexp 2)
            (if (looking-at "\\<switch\\>") '/))
        (error)))))

(require 'cc-styles)
(c-add-style
 "ruby"
 '("bsd"
   (c-basic-offset . 4)
   (tab-width . 8)
   (indent-tabs-mode . t)
   (setq show-trailing-whitespace t)
   (c-offsets-alist
    (case-label . *)
    (label . (ruby-style-label-indent *))
    (statement-case-intro . *)
    (statement-case-open . *)
    (statement-block-intro . (ruby-style-case-indent +))
    (access-label /)
    )))

(defun ruby-style-c-mode ()
  (interactive)
  (if (or (let ((name (buffer-file-name))) (and name (string-match "/ruby\\>" name)))
          (save-excursion
            (goto-char (point-min))
            (let ((head (progn (forward-line 100) (point)))
                  (case-fold-search nil))
              (goto-char (point-min))
              (re-search-forward "Copyright (C) .* Yukihiro Matsumoto" head t))))
      (c-set-style "ruby")))

(provide 'ruby-style)
