(defun lambda-mode ()
  (interactive)
  (fundamental-mode)
  (setq major-mode 'lambda-mode)
  (setq mode-name "Lambda")
  (make-variable-buffer-local 'font-lock-defaults)
  (if (or (string-match "^19\\." emacs-version)
	  (string-match "^20\\.[12]\\(\\.\\|$\\)" emacs-version))
      (setq font-lock-constant-face 'font-lock-reference-face))
  (setq font-lock-defaults
	'((lambda-font-lock-keywords)
	  nil nil ((?% . "<") (?\n . ">")) nil))
  )

(defvar lambda-font-lock-keywords
  '(("\\<\\(lambda\\|define\\)\\>" 1 font-lock-keyword-face)
    ("\\<define\\>\\s +\\([a-z]\\w*\\)" 1 font-lock-function-name-face)
    ("\\<\\([a-z]\\w*\\)\\>" 1 font-lock-variable-name-face)
    ("\\([0-9]+\\)" 1 font-lock-constant-face)
    ))
