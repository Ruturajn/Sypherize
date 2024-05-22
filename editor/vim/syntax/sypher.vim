" Syntax highlighting for Sypher
" Language: sypher
" Latest Revision: 2024-05-22

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
    \,                  "string"
    \,                  "bool"
    \,                 ]
    \,  'sypherLoop' : ["for"
    \,                  "while"
    \,                 ]
    \,  'sypherBoolean' : ["true"
    \,                     "false"
    \,                    ]
    \,  'sypherKeyword' : ["fun"
    \,                     "new"
    \,                    ]
    \, }

function! s:syntax_keyword(dict)
    for key in keys(a:dict)
        execute 'syntax keyword' key join(a:dict[key], ' ')
    endfor
endfunction

call s:syntax_keyword(s:sypher_syntax_keywords)

" Match the digits as many as possible.
syntax match sypherNumber display "\v<\d+>"

" Match function return operator
syntax match sypherArrowOperator display "\V->"

" Match the operators defined within the [].
syntax match sypherOperator display "\V\[-+/:=*@#&<>,;]"

" Match words as many as possible followed by multiple line spaces
" and then an opening parenthesis.
syntax match sypherFunction /\w\+\s*(/me=e-1,he=e-1

" Match words as many as possible followed by multiple line spaces
" and then either a comma or a close brace.
syntax match sypherIdentifier /\w\+\s*[,)]/me=e-1,he=e-1

syntax region sypherCommentOneLine start="\$" end="$"
syntax region sypherCommentMultiLine start="(\^" end="\^)" contains=sypherCommentMultiLine

syntax region sypherString matchgroup=sypherStringDelimiter start=+"+ skip=+\\\\\|\\"+ end=+"+ oneline contains=sypherEscape
syntax region sypherChar matchgroup=sypherCharDelimiter start=+'+ skip=+\\\\\|\\'+ end=+'+ oneline contains=sypherEscape
syntax match sypherEscape        display contained /\\./

highlight default link sypherConditional        Conditional
highlight default link sypherType               Type
highlight default link sypherCommentOneLine     Comment
highlight default link sypherCommentMultiLine   Comment
highlight default link sypherCommentLineTwoNest Comment
highlight default link sypherNumber             Number
highlight default link sypherArrowOperator      sypherOperator
highlight default link sypherOperator           Operator
highlight default link sypherFunction           Function
highlight default link sypherIdentifier         Identifier
highlight default link sypherLoop               Repeat
highlight default link sypherBoolean            Boolean
highlight default link sypherKeyword            Keyword
highlight default link sypherString             String
highlight default link sypherStringDelimiter    String
highlight default link sypherChar               String
highlight default link sypherCharDelimiter      String
highlight default link sypherEscape             Special


delfunction s:syntax_keyword

let b:current_syntax = "sypher"

let &cpo = s:cpo_save
unlet! s:cpo_save
