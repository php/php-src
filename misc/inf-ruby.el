;;; -*-Emacs-Lisp-*-
;;;
;;;  $Id$
;;;  $Author$
;;;
;;; Inferior Ruby Mode - ruby process in a buffer.
;;;                      adapted from cmuscheme.el
;;;
;;; Usage:
;;;
;;; (0) check ruby-program-name variable that can run your environment.
;;;
;;; (1) modify .emacs to use ruby-mode
;;;     for example :
;;;
;;;    (autoload 'ruby-mode "ruby-mode"
;;;      "Mode for editing ruby source files" t)
;;;    (setq auto-mode-alist
;;;          (append '(("\\.rb$" . ruby-mode)) auto-mode-alist))
;;;    (setq interpreter-mode-alist (append '(("ruby" . ruby-mode))
;;;                                  interpreter-mode-alist))
;;;
;;; (2) set to load inf-ruby and set inf-ruby key definition in ruby-mode.
;;;
;;;    (autoload 'run-ruby "inf-ruby"
;;;      "Run an inferior Ruby process")
;;;    (autoload 'inf-ruby-keys "inf-ruby"
;;;      "Set local key defs for inf-ruby in ruby-mode")
;;;    (add-hook 'ruby-mode-hook
;;;          '(lambda ()
;;;             (inf-ruby-keys)
;;;    ))
;;;
;;; HISTORY
;;; senda -  8 Apr 1998: Created.
;;;      $Log$
;;;      Revision 1.7  2004/07/27 08:11:36  matz
;;;      * eval.c (rb_eval): copy on write for argument local variable
;;;        assignment.
;;;
;;;      * eval.c (assign): ditto.
;;;
;;;      * eval.c (rb_call0): update ruby_frame->argv with the default
;;;        value used for the optional arguments.
;;;
;;;      * object.c (Init_Object): "===" calls rb_obj_equal() directly.
;;;        [ruby-list:39937]
;;;
;;;      Revision 1.6  2002/09/07 14:35:46  nobu
;;;      * misc/inf-ruby.el (inferior-ruby-error-regexp-alist): regexp
;;;        alist for error message from ruby.
;;;
;;;      * misc/inf-ruby.el (inferior-ruby-mode): fixed for Emacs.
;;;
;;;      * misc/inf-ruby.el (ruby-send-region): compilation-parse-errors
;;;        doesn't parse first line, so insert separators before each
;;;        evaluations.
;;;
;;;      Revision 1.5  2002/08/19 10:05:47  nobu
;;;      * misc/inf-ruby.el (inf-ruby-keys): ruby-send-definition
;;;        conflicted with ruby-insert-end.
;;;
;;;      * misc/inf-ruby.el (inferior-ruby-mode): compilation-minor-mode.
;;;
;;;      * misc/inf-ruby.el (ruby-send-region): send as here document to
;;;        adjust source file/line.  [ruby-talk:47113], [ruby-dev:17965]
;;;
;;;      * misc/inf-ruby.el (ruby-send-terminator): added to make unique
;;;        terminator.
;;;
;;;      Revision 1.4  2002/01/29 07:16:09  matz
;;;      * file.c (rb_stat_rdev_major): added. [new]
;;;
;;;      * file.c (rb_stat_rdev_minor): added. [new]
;;;
;;;      * file.c (rb_stat_inspect): print mode in octal.
;;;
;;;      Revision 1.3  1999/12/01 09:24:18  matz
;;;      19991201
;;;
;;;      Revision 1.2  1999/08/13 05:45:18  matz
;;;      1.4.0
;;;
;;;      Revision 1.1.1.1.2.1  1999/07/15 07:59:59  matz
;;;      990715
;;;
;;;      Revision 1.1.1.1  1999/01/20 04:59:36  matz
;;;      ruby 1.3 cycle
;;;
;;;      Revision 1.1.2.1  1998/12/16 07:30:36  matz
;;;      first public release of 1.1d (pre1.2) series
;;;
;;;      Revision 1.4  1998/05/20 02:45:58  senda
;;;      default program to irb
;;;
;;;      Revision 1.3  1998/04/10 04:11:30  senda
;;;      modification by Matsumoto san (1.1b9_09)
;;;      remove-in-string defined
;;;      global variable :
;;;              inferior-ruby-first-prompt-pattern
;;;            inferior-ruby-prompt-pattern
;;;      defined
;;;
;;;      Revision 1.2  1998/04/09 07:53:42  senda
;;;      remove M-C-x in inferior-ruby-mode
;;;
;;;      Revision 1.1  1998/04/09 07:28:36  senda
;;;      Initial revision
;;;
;;;

(require 'comint)
(require 'compile)
(require 'ruby-mode)

;;
;; you may change these variables
;;
;(defvar ruby-program-name "rbc --noreadline"
;  "*Program invoked by the run-ruby command")
;
;(defvar inferior-ruby-first-prompt-pattern "^rbc0> *"
;  "first prompt regex pattern of ruby interpreter.")
;
;(defvar inferior-ruby-prompt-pattern "^\\(rbc.[>*\"'] *\\)+"
;  "prompt regex pattern of ruby interpreter.")

;;;; for irb
(defvar ruby-program-name "irb --inf-ruby-mode"
  "*Program invoked by the run-ruby command")

(defvar inferior-ruby-first-prompt-pattern "^irb(.*)[0-9:]+0> *"
  "first prompt regex pattern of ruby interpreter.")

(defvar inferior-ruby-prompt-pattern "^\\(irb(.*)[0-9:]+[>*\"'] *\\)+"
  "prompt regex pattern of ruby interpreter.")

;;
;; mode variables
;;
(defvar inferior-ruby-mode-hook nil
  "*Hook for customising inferior-ruby mode.")
(defvar inferior-ruby-mode-map nil
  "*Mode map for inferior-ruby-mode")

(defconst inferior-ruby-error-regexp-alist
       '(("SyntaxError: compile error\n^\\([^\(].*\\):\\([1-9][0-9]*\\):" 1 2)
         ("^\tfrom \\([^\(].*\\):\\([1-9][0-9]*\\)\\(:in `.*'\\)?$" 1 2)))

(cond ((not inferior-ruby-mode-map)
       (setq inferior-ruby-mode-map
             (copy-keymap comint-mode-map))
;       (define-key inferior-ruby-mode-map "\M-\C-x" ;gnu convention
;                  'ruby-send-definition)
;       (define-key inferior-ruby-mode-map "\C-x\C-e" 'ruby-send-last-sexp)
       (define-key inferior-ruby-mode-map "\C-c\C-l" 'ruby-load-file)
))

(defun inf-ruby-keys ()
  "Set local key defs for inf-ruby in ruby-mode"
  (define-key ruby-mode-map "\M-\C-x" 'ruby-send-definition)
;  (define-key ruby-mode-map "\C-x\C-e" 'ruby-send-last-sexp)
  (define-key ruby-mode-map "\C-c\C-b" 'ruby-send-block)
  (define-key ruby-mode-map "\C-c\M-b" 'ruby-send-block-and-go)
  (define-key ruby-mode-map "\C-c\C-x" 'ruby-send-definition)
  (define-key ruby-mode-map "\C-c\M-x" 'ruby-send-definition-and-go)
  (define-key ruby-mode-map "\C-c\C-r" 'ruby-send-region)
  (define-key ruby-mode-map "\C-c\M-r" 'ruby-send-region-and-go)
  (define-key ruby-mode-map "\C-c\C-z" 'switch-to-ruby)
  (define-key ruby-mode-map "\C-c\C-l" 'ruby-load-file)
  (define-key ruby-mode-map "\C-c\C-s" 'run-ruby)
)

(defvar ruby-buffer nil "current ruby (actually irb) process buffer.")

(defun inferior-ruby-mode ()
  "Major mode for interacting with an inferior ruby (irb) process.

The following commands are available:
\\{inferior-ruby-mode-map}

A ruby process can be fired up with M-x run-ruby.

Customisation: Entry to this mode runs the hooks on comint-mode-hook and
inferior-ruby-mode-hook (in that order).

You can send text to the inferior ruby process from other buffers containing
Ruby source.
    switch-to-ruby switches the current buffer to the ruby process buffer.
    ruby-send-definition sends the current definition to the ruby process.
    ruby-send-region sends the current region to the ruby process.

    ruby-send-definition-and-go, ruby-send-region-and-go,
        switch to the ruby process buffer after sending their text.
For information on running multiple processes in multiple buffers, see
documentation for variable ruby-buffer.

Commands:
Return after the end of the process' output sends the text from the
    end of process to point.
Return before the end of the process' output copies the sexp ending at point
    to the end of the process' output, and sends it.
Delete converts tabs to spaces as it moves back.
Tab indents for ruby; with argument, shifts rest
    of expression rigidly with the current line.
C-M-q does Tab on each line starting within following expression.
Paragraphs are separated only by blank lines.  # start comments.
If you accidentally suspend your process, use \\[comint-continue-subjob]
to continue it."
  (interactive)
  (comint-mode)
  ;; Customise in inferior-ruby-mode-hook
  ;(setq comint-prompt-regexp "^[^>\n]*>+ *")
  (setq comint-prompt-regexp inferior-ruby-prompt-pattern)
  ;;(scheme-mode-variables)
  (ruby-mode-variables)
  (setq major-mode 'inferior-ruby-mode)
  (setq mode-name "Inferior Ruby")
  (setq mode-line-process '(":%s"))
  (use-local-map inferior-ruby-mode-map)
  (setq comint-input-filter (function ruby-input-filter))
  (setq comint-get-old-input (function ruby-get-old-input))
  (compilation-shell-minor-mode t)
  (make-local-variable 'compilation-error-regexp-alist)
  (setq compilation-error-regexp-alist inferior-ruby-error-regexp-alist)
  (run-hooks 'inferior-ruby-mode-hook))

(defvar inferior-ruby-filter-regexp "\\`\\s *\\S ?\\S ?\\s *\\'"
  "*Input matching this regexp are not saved on the history list.
Defaults to a regexp ignoring all inputs of 0, 1, or 2 letters.")

(defun ruby-input-filter (str)
  "Don't save anything matching inferior-ruby-filter-regexp"
  (not (string-match inferior-ruby-filter-regexp str)))

;; adapted from replace-in-string in XEmacs (subr.el)
(defun remove-in-string (str regexp)
  "Remove all matches in STR for REGEXP and returns the new string."
  (let ((rtn-str "") (start 0) match prev-start)
    (while (setq match (string-match regexp str start))
      (setq prev-start start
            start (match-end 0)
            rtn-str (concat rtn-str (substring str prev-start match))))
    (concat rtn-str (substring str start))))

(defun ruby-get-old-input ()
  "Snarf the sexp ending at point"
  (save-excursion
    (let ((end (point)))
      (re-search-backward inferior-ruby-first-prompt-pattern)
      (remove-in-string (buffer-substring (point) end)
                        inferior-ruby-prompt-pattern)
      )))

(defun ruby-args-to-list (string)
  (let ((where (string-match "[ \t]" string)))
    (cond ((null where) (list string))
          ((not (= where 0))
           (cons (substring string 0 where)
                 (ruby-args-to-list (substring string (+ 1 where)
                                                 (length string)))))
          (t (let ((pos (string-match "[^ \t]" string)))
               (if (null pos)
                   nil
                 (ruby-args-to-list (substring string pos
                                                 (length string)))))))))

(defun run-ruby (cmd)
  "Run an inferior Ruby process, input and output via buffer *ruby*.
If there is a process already running in `*ruby*', switch to that buffer.
With argument, allows you to edit the command line (default is value
of `ruby-program-name').  Runs the hooks `inferior-ruby-mode-hook'
\(after the `comint-mode-hook' is run).
\(Type \\[describe-mode] in the process buffer for a list of commands.)"

  (interactive (list (if current-prefix-arg
                         (read-string "Run Ruby: " ruby-program-name)
                         ruby-program-name)))
  (if (not (comint-check-proc "*ruby*"))
      (let ((cmdlist (ruby-args-to-list cmd)))
        (set-buffer (apply 'make-comint "ruby" (car cmdlist)
                           nil (cdr cmdlist)))
        (inferior-ruby-mode)))
  (setq ruby-program-name cmd)
  (setq ruby-buffer "*ruby*")
  (pop-to-buffer "*ruby*"))

(defconst ruby-send-terminator "--inf-ruby-%x-%d-%d-%d--"
  "Template for irb here document terminator.
Must not contain ruby meta characters.")

(defconst ruby-eval-separator "")

(defun ruby-send-region (start end)
  "Send the current region to the inferior Ruby process."
  (interactive "r")
  (let (term (file (buffer-file-name)) line)
    (save-excursion
      (save-restriction
        (widen)
        (goto-char start)
        (setq line (+ start (forward-line (- start)) 1))
        (goto-char start)
        (while (progn
                 (setq term (apply 'format ruby-send-terminator (random) (current-time)))
                 (re-search-forward (concat "^" (regexp-quote term) "$") end t)))))
    ;; compilation-parse-errors parses from second line.
    (save-excursion
      (let ((m (process-mark (ruby-proc))))
        (set-buffer (marker-buffer m))
        (goto-char m)
        (insert ruby-eval-separator "\n")
        (set-marker m (point))))
    (comint-send-string (ruby-proc) (format "eval <<'%s', nil, %S, %d\n" term file line))
    (comint-send-region (ruby-proc) start end)
    (comint-send-string (ruby-proc) (concat "\n" term "\n"))))

(defun ruby-send-definition ()
  "Send the current definition to the inferior Ruby process."
  (interactive)
  (save-excursion
    (ruby-end-of-defun)
    (let ((end (point)))
      (ruby-beginning-of-defun)
      (ruby-send-region (point) end))))

;(defun ruby-send-last-sexp ()
;  "Send the previous sexp to the inferior Ruby process."
;  (interactive)
;  (ruby-send-region (save-excursion (backward-sexp) (point)) (point)))

(defun ruby-send-block ()
  "Send the current block to the inferior Ruby process."
  (interactive)
  (save-excursion
    (ruby-end-of-block)
    (end-of-line)
    (let ((end (point)))
      (ruby-beginning-of-block)
      (ruby-send-region (point) end))))

(defun switch-to-ruby (eob-p)
  "Switch to the ruby process buffer.
With argument, positions cursor at end of buffer."
  (interactive "P")
  (if (get-buffer ruby-buffer)
      (pop-to-buffer ruby-buffer)
      (error "No current process buffer. See variable ruby-buffer."))
  (cond (eob-p
         (push-mark)
         (goto-char (point-max)))))

(defun ruby-send-region-and-go (start end)
  "Send the current region to the inferior Ruby process.
Then switch to the process buffer."
  (interactive "r")
  (ruby-send-region start end)
  (switch-to-ruby t))

(defun ruby-send-definition-and-go ()
  "Send the current definition to the inferior Ruby.
Then switch to the process buffer."
  (interactive)
  (ruby-send-definition)
  (switch-to-ruby t))

(defun ruby-send-block-and-go ()
  "Send the current block to the inferior Ruby.
Then switch to the process buffer."
  (interactive)
  (ruby-send-block)
  (switch-to-ruby t))

(defvar ruby-source-modes '(ruby-mode)
  "*Used to determine if a buffer contains Ruby source code.
If it's loaded into a buffer that is in one of these major modes, it's
considered a ruby source file by ruby-load-file.
Used by these commands to determine defaults.")

(defvar ruby-prev-l/c-dir/file nil
  "Caches the last (directory . file) pair.
Caches the last pair used in the last ruby-load-file command.
Used for determining the default in the
next one.")

(defun ruby-load-file (file-name)
  "Load a Ruby file into the inferior Ruby process."
  (interactive (comint-get-source "Load Ruby file: " ruby-prev-l/c-dir/file
                                  ruby-source-modes t)) ; T because LOAD
                                                          ; needs an exact name
  (comint-check-source file-name) ; Check to see if buffer needs saved.
  (setq ruby-prev-l/c-dir/file (cons (file-name-directory    file-name)
                                       (file-name-nondirectory file-name)))
  (comint-send-string (ruby-proc) (concat "(load \""
                                            file-name
                                            "\"\)\n")))

(defun ruby-proc ()
  "Returns the current ruby process. See variable ruby-buffer."
  (let ((proc (get-buffer-process (if (eq major-mode 'inferior-ruby-mode)
                                      (current-buffer)
                                    ruby-buffer))))
    (or proc
        (error "No current process. See variable ruby-buffer"))))

;;; Do the user's customisation...

(defvar inf-ruby-load-hook nil
  "This hook is run when inf-ruby is loaded in.
This is a good place to put keybindings.")

(run-hooks 'inf-ruby-load-hook)

(provide 'inf-ruby)

;;; inf-ruby.el ends here
