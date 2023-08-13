" Syntax highlighting for Sypher
" Language: sypher
" Latest Revision: 2023-08-13

if exists("b:current_syntax")
    finish
endif

let s:cpo_save = &cpo
set cpo&vim

let s:sypher_syntax_keywords = {
    \   'sypherConditional' : ["if"
    \,                         "else"
    \,                        ]
    \,  'sypherType' : ["int"
    \,                 ]
    \, }

function! s:syntax_keyword(dict)
    for key in keys(a:dict)
        execute 'syntax keyword' key join(a:dict[key], ' ')
    endfor
endfunction

call s:syntax_keyword(s:sypher_syntax_keywords)

syntax region sypherCommentLineOne start="`" end="$"
syntax region sypherCommentLineTwo start="#" end="$"

highlight default link sypherConditional    Conditional
highlight default link sypherType           Type
highlight default link sypherCommentLineOne Comment
highlight default link sypherCommentLineTwo Comment


delfunction s:syntax_keyword

let b:current_syntax = "sypher"

let &cpo = s:cpo_save
unlet! s:cpo_save
