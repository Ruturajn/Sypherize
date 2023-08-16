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

" Match the digits as many as possible.
syntax match sypherNumber display "\v<\d+>"

" Match the operators defined within the [].
syntax match sypherOperator display "\V\[-+/:=*@&<>,;]"

" Match words as many as possible followed by multiple line spaces
" and then an opening parenthesis.
syntax match sypherFunction /\w\+\s*(/me=e-1,he=e-1

" Match words as many as possible followed by multiple line spaces
" and then either a comma or a close brace.
syntax match sypherIdentifier /\w\+\s*[,)]/me=e-1,he=e-1

syntax region sypherCommentLineOne start="`" end="$"
syntax region sypherCommentLineTwo start="#" end="$"

highlight default link sypherConditional    Conditional
highlight default link sypherType           Type
highlight default link sypherCommentLineOne Comment
highlight default link sypherCommentLineTwo Comment
highlight default link sypherNumber         Number
highlight default link sypherOperator       Operator
highlight default link sypherFunction       Function
highlight default link sypherIdentifier     Identifier

delfunction s:syntax_keyword

let b:current_syntax = "sypher"

let &cpo = s:cpo_save
unlet! s:cpo_save
