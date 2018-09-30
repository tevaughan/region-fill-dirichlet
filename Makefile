
regfill.pdf : regfill.tex
	pdflatex $<
	pdflatex $<

.PHONY: clean

clean :
	@rm -fv *.log
	@rm -fv *.aux
	@rm -fv *.out
	@rm -fv regfill.pdf

