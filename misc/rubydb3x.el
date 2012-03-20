(require 'gud)
(provide 'rubydb)

;; ======================================================================
;; rubydb functions

;;; History of argument lists passed to rubydb.
(defvar gud-rubydb-history nil)

(if (fboundp 'gud-overload-functions)
    (defun gud-rubydb-massage-args (file args)
      (cons "-r" (cons "debug" (cons file args))))
  (defun gud-rubydb-massage-args (file args)
      (cons "-r" (cons "debug" args))))

;; There's no guarantee that Emacs will hand the filter the entire
;; marker at once; it could be broken up across several strings.  We
;; might even receive a big chunk with several markers in it.  If we
;; receive a chunk of text which looks like it might contain the
;; beginning of a marker, we save it here between calls to the
;; filter.
(defvar gud-rubydb-marker-acc "")
(make-variable-buffer-local 'gud-rubydb-marker-acc)

(defun gud-rubydb-marker-filter (string)
  (setq gud-rubydb-marker-acc (concat gud-rubydb-marker-acc string))
  (let ((output ""))

    ;; Process all the complete markers in this chunk.
    (while (string-match "\032\032\\([^:\n]*\\):\\([0-9]*\\):.*\n"
                         gud-rubydb-marker-acc)
      (setq

       ;; Extract the frame position from the marker.
       gud-last-frame
       (cons (substring gud-rubydb-marker-acc (match-beginning 1) (match-end 1))
             (string-to-int (substring gud-rubydb-marker-acc
                                       (match-beginning 2)
                                       (match-end 2))))

       ;; Append any text before the marker to the output we're going
       ;; to return - we don't include the marker in this text.
       output (concat output
                      (substring gud-rubydb-marker-acc 0 (match-beginning 0)))

       ;; Set the accumulator to the remaining text.
       gud-rubydb-marker-acc (substring gud-rubydb-marker-acc (match-end 0))))

    ;; Does the remaining text look like it might end with the
    ;; beginning of another marker?  If it does, then keep it in
    ;; gud-rubydb-marker-acc until we receive the rest of it.  Since we
    ;; know the full marker regexp above failed, it's pretty simple to
    ;; test for marker starts.
    (if (string-match "\032.*\\'" gud-rubydb-marker-acc)
        (progn
          ;; Everything before the potential marker start can be output.
          (setq output (concat output (substring gud-rubydb-marker-acc
                                                 0 (match-beginning 0))))

          ;; Everything after, we save, to combine with later input.
          (setq gud-rubydb-marker-acc
                (substring gud-rubydb-marker-acc (match-beginning 0))))

      (setq output (concat output gud-rubydb-marker-acc)
            gud-rubydb-marker-acc ""))

    output))

(defun gud-rubydb-find-file (f)
  (save-excursion
    (let ((buf (find-file-noselect f)))
      (set-buffer buf)
;;      (gud-make-debug-menu)
      buf)))

(defvar rubydb-command-name "ruby"
  "File name for executing ruby.")

;;;###autoload
(defun rubydb (command-line)
  "Run rubydb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run rubydb (like this): "
                               (if (consp gud-rubydb-history)
                                   (car gud-rubydb-history)
                                 (concat rubydb-command-name " "))
                               nil nil
                               '(gud-rubydb-history . 1))))

  (if (not (fboundp 'gud-overload-functions))
      (gud-common-init command-line 'gud-rubydb-massage-args
                       'gud-rubydb-marker-filter 'gud-rubydb-find-file)
    (gud-overload-functions '((gud-massage-args . gud-rubydb-massage-args)
                              (gud-marker-filter . gud-rubydb-marker-filter)
                              (gud-find-file . gud-rubydb-find-file)))
    (gud-common-init command-line rubydb-command-name))

  (gud-def gud-break  "b %l"         "\C-b" "Set breakpoint at current line.")
;  (gud-def gud-remove "clear %l"     "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "s"            "\C-s" "Step one source line with display.")
  (gud-def gud-next   "n"            "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "c"            "\C-r" "Continue with display.")
  (gud-def gud-finish "finish"       "\C-f" "Finish executing current function.")
  (gud-def gud-up     "up %p"        "<" "Up N stack frames (numeric arg).")
  (gud-def gud-down   "down %p"      ">" "Down N stack frames (numeric arg).")
  (gud-def gud-print  "p %e"         "\C-p" "Evaluate ruby expression at point.")

  (setq comint-prompt-regexp "^(rdb:-) ")
  (if (boundp 'comint-last-output-start)
      (set-marker comint-last-output-start (point)))
  (set (make-local-variable 'paragraph-start) comint-prompt-regexp)
  (run-hooks 'rubydb-mode-hook)
  )
