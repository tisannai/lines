(list
 (cons "lines"
       (list (cons 'project-type                 "c-executable")
             (cons 'source-directory             "src")
             (cons 'version                      "0.0.1")
             (cons 'compile-options              (list "-g" "-Wall"))
             ;; (cons 'compile-options              (list "-O2"))
             (cons 'library-dependencies         (list))
             (cons 'actions                      (list "compile" "link" "publish"))))
 )
