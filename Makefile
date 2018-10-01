
regfill.pdf : regfill.tex trees-raw.pgm
	pdflatex $<
	pdflatex $<

trees-raw.pgm : trees.pgm
	pnmcat -lr trees.pgm > trees-raw.pgm

.PHONY: clean

clean :
	@rm -fv *.log
	@rm -fv *.aux
	@rm -fv *.out
	@rm -fv regfill.pdf
	@rm -fv trees-raw.pgm

