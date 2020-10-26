# nechajte iba jeden z main.pdf a main-en.pdf
all: main-en.pdf clean

main-en.pdf: main-en.tex *.tex *.bib images/*
	pdflatex main-en
	bibtex main-en
	pdflatex main-en
	pdflatex main-en

clean:
	rm -f main-en.aux main-en.bbl main-en.blg main-en.aux main-en.lof main-en.lot main-en.toc main-en.log main-en.out

#main.pdf: main.tex *.tex *.bib images/*
#	pdflatex main
#	bibtex main
#	pdflatex main
#	pdflatex main
