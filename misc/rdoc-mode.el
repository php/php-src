;;
;; rdoc-mode.el
;; Major mode for RDoc editing
;;

;; Created: Fri Sep 18 09:04:49 JST 2009

;; License: Ruby's

(require 'derived)
(define-derived-mode rdoc-mode text-mode "RDoc"
  "Major mode for RD editing.
\\{rdoc-mode-map}"
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate "^\\(=+\\|\\*+\\)[ \t\v\f]*\\|^\\s *$")
  (make-local-variable 'paragraph-start)
  (setq paragraph-start paragraph-separate)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '((rdoc-font-lock-keywords) t nil))
  (make-local-variable 'font-lock-keywords)
  (setq font-lock-keywords rdoc-font-lock-keywords)
  (make-local-variable 'outline-regexp)
  (setq outline-regexp "^\\(=+\\)[ \t\v\f]*")
  (outline-minor-mode t)
  (setq show-trailing-whitespace t)
  (rdoc-setup-keys)
  (setq indent-tabs-mode nil)
  (run-hooks 'rdoc-mode-hook)
  )

(defun rdoc-fill-paragraph (&rest args)
  "Fills paragraph, except for cited region"
  (interactive (progn
		 (barf-if-buffer-read-only)
		 (list (if current-prefix-arg 'full))))
  (save-excursion
    (beginning-of-line)
    (unless (looking-at "^ +")
      (apply 'fill-paragraph args))))

(defun rdoc-setup-keys ()
  (interactive)
  (define-key rdoc-mode-map "\M-q" 'rdoc-fill-paragraph)
  )

(defvar rdoc-heading1-face 'font-lock-keywordoc-face)
(defvar rdoc-heading2-face 'font-lock-type-face)
(defvar rdoc-heading3-face 'font-lock-variable-name-face)
(defvar rdoc-heading4-face 'font-lock-comment-face)
(defvar rdoc-bold-face 'font-lock-function-name-face)
(defvar rdoc-emphasis-face 'font-lock-function-name-face)
(defvar rdoc-code-face 'font-lock-keyword-face)
(defvar rdoc-description-face 'font-lock-constant-face)

(defvar rdoc-font-lock-keywords
  (list
   (list "^=([^=\r\n].*)?$"
	 0 rdoc-heading1-face)
   (list "^==([^=\r\n].*)?$"
	 0 rdoc-heading2-face)
   (list "^===([^=\r\n].*)?$"
	 0 rdoc-heading3-face)
   (list "^====+.*$"
	 0 rdoc-heading4-face)
   (list "\\(^\\|[ \t\v\f]\\)\\(\\*\\(\\sw\\|[-_:]\\)+\\*\\)\\($\\|[ \t\v\f]\\)"
	 2 rdoc-bold-face)		; *bold*
   (list "\\(^\\|[ \t\v\f]\\)\\(_\\(\\sw\\|[-_:]\\)+_\\)\\($\\|[ \t\v\f]\\)"
	 2 rdoc-emphasis-face)		; _emphasis_
   (list "\\(^\\|[ \t\v\f]\\)\\(\\+\\(\\sw\\|[-_:]\\)+\\+\\)\\($\\|[ \t\v\f]\\)"
	 2 rdoc-code-face)		; +code+
   (list "<em>[^<>]*</em>" 0 rdoc-emphasis-face)
   (list "<i>[^<>]*</i>" 0 rdoc-emphasis-face)
   (list "<b>[^<>]*</b>" 0 rdoc-bold-face)
   (list "<tt>[^<>]*</tt>" 0 rdoc-code-face)
   (list "<code>[^<>]*</code>" 0 rdoc-code-face)
   (list "^\\([-*]\\|[0-9]+\\.\\|[A-Za-z]\\.\\)\\s "
	 1 rdoc-description-face) ; bullet | numbered | alphabetically numbered
   (list "^\\[[^\]]*\\]\\|\\S .*::\\)\\([ \t\v\f]\\|$\\)"
	 1 rdoc-description-face)	; labeled | node
   ;(list "^[ \t\v\f]+\\(.*\\)" 1 rdoc-verbatim-face)
   ))

(defun rdoc-imenu-create-index ()
  (let ((root '(nil . nil))
        cur-alist
        (cur-level 0)
        (pattern (concat outline-regexp "\\(.*?\\)[ \t\v\f]*$"))
        (empty-heading "-")
        (self-heading ".")
        pos level heading alist)
    (save-excursion
      (goto-char (point-min))
      (while (re-search-forward pattern (point-max) t)
        (setq heading (match-string-no-properties 2)
              level (min 6 (length (match-string-no-properties 1)))
              pos (match-beginning 1))
        (if (= (length heading) 0)
            (setq heading empty-heading))
        (setq alist (list (cons heading pos)))
        (cond
         ((= cur-level level)		; new sibling
          (setcdr cur-alist alist)
          (setq cur-alist alist))
         ((< cur-level level)		; first child
          (dotimes (i (- level cur-level 1))
            (setq alist (list (cons empty-heading alist))))
          (if cur-alist
              (let* ((parent (car cur-alist))
                     (self-pos (cdr parent)))
                (setcdr parent (cons (cons self-heading self-pos) alist)))
            (setcdr root alist))	; primogenitor
          (setq cur-alist alist
                cur-level level))
         (t				; new sibling of an ancestor
          (let ((sibling-alist (last (cdr root))))
            (dotimes (i (1- level))
              (setq sibling-alist (last (cdar sibling-alist))))
            (setcdr sibling-alist alist)
            (setq cur-alist alist
                  cur-level level))))))
    (cdr root)))

(defun rdoc-set-imenu-create-index-function ()
  (setq imenu-create-index-function 'rdoc-imenu-create-index))

(add-hook 'rdoc-mode-hook 'rdoc-set-imenu-create-index-function)

(provide 'rdoc-mode)
