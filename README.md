SELECT nome
	FROM A;
SELECT nome,tel
	FROM A;
SELECT nome,tel
	FROM A
	WHERE nome='marcelo';
SELECT nome,tipo,tel
	FROM (A JOIN B ON x=y);
SELECT nome,tipo
	FROM (A JOIN B ON x=y)
	WHERE nome='marcelo';
