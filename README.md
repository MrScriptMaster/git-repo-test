![Logo](https://upload.wikimedia.org/wikipedia/commons/5/54/GitHub_Logo.png "GitHub Logo")
# Преамбула
В этом файле представлен синтаксис разметки типового *.md файла, который может быть просмотрен в репозитории через веб-браузер.

## Элементы разметки
Чтобы вставить заголовок в текст, воспользуйтесь символом решетки ('#'). Установите несколько "решеток" подряд, чтобы установить глубину заголовка.

Для формирования абзацев просто оставляйте пустую строку между ними. Для вставки в текст списка используйте тире для формиования пунктов. Например так:
- Первый пункт
- Второй пункт
- Третий пункт
- Четвертый пункт

Внутри файла вы можете использовать комментарии (пара'<!--' '-->')

### Примеры
Гиперссылка: [GitHub](https://ru.wikipedia.org/wiki/GitHub)

Ссылка на каталог репзоитория: [Docs](docs/)

Ссылка на другой md-файл: [Page 1](docs/page1.md)

Ссылка без надписи: https://ru.wikipedia.org/wiki/GitHub

Для выделения команд интерпретатора удобно использовать боксы. Для этого просто воспользуйтесь табулятором.

	$ ls -l
	
	$ echo "Hello, World!"

Можно использовать подсветку синтаксиса в боксах, если воспользоваться специальными символами разметки

```sh
if [ -e doc ]; then
	cp -vf $FROM $TO
fi
```

Можно вставлять фрагменты кода и также подсвечивать синтаксис.

```
	class MyClass {
		enum {
			size=100
		};
		
		int mass[size];
	public:
		int getValue(unsigned int index) const
		{
			if(index >= size)
				return -1;
			else
				return mass[index];
		}
	};
```