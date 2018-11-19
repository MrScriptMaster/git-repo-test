Some interpreters may form a table of contents.

[TOC]

# Headline 1
## Headline 2
### Headline 3
#### Headline 4
##### Headline 5
###### Headline 6
# Headline with underline, variant 1
======================================
WARNING: Not all visualizers interpret this as an underscore. Use variant 2.
 
 
# Headline with underline, variant 2
--------------------------------------
*Italic 1*, _Italic 2_, **Bold 1**, __Bold 2__, **mixed _outline_**, ~~strikethrough~~.
 
 
## Lists ##
1. Item 1
2. Item 2
   * Sub item 1
   * Sub item 2
3. Item 3
   1. Sub item 3
   2. Sub item 4
4. Item 4
 
 
   Do not forget about empty line if you want insert an indent to a list.
   This text on the new line.
 
 
For lists you can use '+', '-' or '*'.
 
 
## Web-links ##
+ [Simple link to google.com](https://www.google.com)
+ [Simple link to google.com with title](https://www.google.com "Go to the Google")
+ [Footnote link][Footnote text]
+ [Relative reference to the document](../blob/LICENSE)
+ [Footnote with number][1]
+ Text with footnote [footnote text]
 
 
[Footnote text]: www.google.com
[1]: www.google.com
[footnote text]: www.google.com
 
 
## Images ##
![Alternative text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Tooltip")
 
 
_Image in footnote_
![Alternative text][logo]
 
 
[logo]: https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Tooltip"
 
 
## Code blocks ##
Use an apostrophe to mark the code in the text, for example: use the `date` command to find out the current date.
 
 
If you want to select a piece of code, then use three apostrophes to select it.
```
int main(int argc, char* argv[]) {
    return 0;
}
```
 
 
Some Markdown interpreters may highlight the syntax of the code. To do this, after the apostrophe, you must specify the name of the language.
```cpp
#include <iostream>
 
int main(int argc, char *argv[]) {
 
  /* An annoying "Hello World" example */
  for (auto i = 0; i < 0xFFFF; i++)
    cout << "Hello, World!" << endl;
 
  char c = '\n';
  unordered_map <string, vector<string> > m;
  m["key"] = "\\\\"; // this is an error
 
  return -2e3 + 12l;
}
```
 
 
## Tables ##
Tables are not part of Markdown, but many interpreters can draw them.
 
 
Vertical lines should be used to designate columns. External vertical lines can not be inserted, but it is still recommended to put them out of the istetic ones. For alignment, the columns use special symbols on the second line.
 
 
| Column 1      | Column 2           | Column 3 |
| ------------- |:------------------:| --------:|
| Row 1         | left               |    $1600 |
| Row 2         | center             |      $12 |
| Row 3         | right              |       $1 |
 
 
## Quotes ##
> To infinity... and beyond. Buzz Lightyear
> (Toy Story)
 
 
Or
 
 
> To infinity... and beyond. Buzz Lightyear
(Toy Story)
 
 
New paragraph.
 
 
## HTML embedded ##
Some Markdown interpreters can visualize html code directly.
 
 
<dl>
  <dt>Bla-bla-bla</dt>
  <dd>Bla-bla-bla-bla-bla-bla</dd>
</dl>
 
 
## Test underscore visualizer ##
 
 
---
 
 
***
 
 
___
 
 
 
 
## Test Youtube-link ##
### Variant 1
<a href="http://www.youtube.com/watch?feature=player_embedded&v=ID_ВИДЕОРОЛИКА_НА_YOUTUBE" target="_blank"><img src="http://img.youtube.com/vi/ID_ВИДЕОРОЛИКА_НА_YOUTUBE/0.jpg"
alt="ALT-ТЕКСТ ИЗОБРАЖЕНИЯ" width="240" height="180" border="10" /></a>
 
 
### Variant 2
[![ALT-ТЕКСТ ИЗОБРАЖЕНИЯ](http://img.youtube.com/vi/ID_ВИДЕОРОЛИКА_НА_YOUTUBE/0.jpg)](http://www.youtube.com/watch?v=ID_ВИДЕОРОЛИКА_НА_YOUTUBE)

## Notifications

!!! important "Important message"
    Line 1
    Line 2
    Line 3

!!! danger "Danger message"
    Danger text

!!! notice "Example notice"
    Notice text

<style type="text/css">
    .block-body {
        background-attachment: scroll;
        background-clip: border-box;
        background-color: rgb(219, 250, 244);
        background-image: none;
        background-origin: padding-box;
        background-position-x: 0%;
        background-position-y: 0%;
        background-repeat-x: ;
        background-repeat-y: ;
        background-size: auto;
        box-sizing: border-box;
        color: rgb(64, 64, 64);
        display: block;
        font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;
        font-size: 14.4px;
        font-weight: 400;
        height: 173px;
        line-height: 24px;
        margin-bottom: 24px;
        padding-bottom: 12px;
        padding-left: 12px;
        padding-right: 12px;
        padding-top: 12px;
        text-size-adjust: 100%;
        width: 368.812px;
        -webkit-font-smoothing: antialiased;
    }
    .admonition-title-important {
        background-attachment: scroll;
        background-clip: border-box;
        background-color: rgb(26, 188, 156);
        background-image: none;
        background-origin: padding-box;
        background-position-x: 0%;
        background-position-y: 0%;
        background-repeat-x: ;
        background-repeat-y: ;
        background-size: auto;
        box-sizing: border-box;
        color: rgb(255, 255, 255);
        display: block;
        font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;
        font-size: 16px;
        font-style: normal;
        font-weight: 700;
        height: 29px;
        line-height: 16px;
        margin-bottom: 12px;
        margin-left: -12px;
        margin-right: -12px;
        margin-top: -12px;
        padding-bottom: 6px;
        padding-left: 12px;
        padding-right: 12px;
        padding-top: 6px;
        text-size-adjust: 100%;
        width: 368.812px;
        -webkit-font-smoothing: antialiased;
        -webkit-margin-after: 12px;
        -webkit-margin-before: -12px;
        -webkit-margin-end: -12px;
        -webkit-margin-start: -12px;
    }
    .admonition-title-danger {
        background-attachment: scroll;
        background-clip: border-box;
        background-color: rgb(242, 159, 151);
        background-image: none;
        background-origin: padding-box;
        background-position-x: 0%;
        background-position-y: 0%;
        background-repeat-x: ;
        background-repeat-y: ;
        background-size: auto;
        box-sizing: border-box;
        color: rgb(255, 255, 255);
        display: block;
        font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;
        font-size: 16px;
        font-style: normal;
        font-weight: 700;
        height: 29px;
        line-height: 16px;
        margin-bottom: 12px;
        margin-left: -12px;
        margin-right: -12px;
        margin-top: -12px;
        padding-bottom: 6px;
        padding-left: 12px;
        padding-right: 12px;
        padding-top: 6px;
        text-size-adjust: 100%;
        width: 368.812px;
        -webkit-font-smoothing: antialiased;
        -webkit-margin-after: 12px;
        -webkit-margin-before: -12px;
        -webkit-margin-end: -12px;
        -webkit-margin-start: -12px;
    }
    .admonition-title-notice {
        background-attachment: scroll;
        background-clip: border-box;
        background-color: rgb(106, 176, 222);
        background-image: none;
        background-origin: padding-box;
        background-position-x: 0%;
        background-position-y: 0%;
        background-repeat-x: ;
        background-repeat-y: ;
        background-size: auto;
        box-sizing: border-box;
        color: rgb(255, 255, 255);
        display: block;
        font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;
        font-size: 16px;
        font-style: normal;
        font-weight: 700;
        height: 29px;
        line-height: 16px;
        margin-bottom: 12px;
        margin-left: -12px;
        margin-right: -12px;
        margin-top: -12px;
        padding-bottom: 6px;
        padding-left: 12px;
        padding-right: 12px;
        padding-top: 6px;
        text-size-adjust: 100%;
        width: 368.812px;
        -webkit-font-smoothing: antialiased;
        -webkit-margin-after: 12px;
        -webkit-margin-before: -12px;
        -webkit-margin-end: -12px;
        -webkit-margin-start: -12px;
    }
    .admonition-title-notice:before, .admonition-title-danger:before,
    .admonition-title-important:before  {
        content: "";
        display: inline-block;
        font-family: FontAwesome;
        font-style: normal;
        font-weight: 400;
        height: 16px;
        line-height: 16px;
        text-decoration-color: rgb(255, 255, 255);
        text-decoration-line: none;
        text-decoration-style: solid;
        width: 13.7188px;
        -webkit-font-smoothing: antialiased;
    }
</style>

<div class="block-body">
<p class="admonition-title-important"> Important message</p>
<p>Text</p>
</div>

<div class="block-body">
<p class="admonition-title-danger"> Danger message</p>
<p>Text</p>
</div>

<div class="block-body">
<p class="admonition-title-notice"> Notice</p>
<p>Text</p>
</div>

<div style="background-attachment: scroll;background-clip: border-box;background-color: rgb(219, 250, 244);
        background-image: none;background-origin: padding-box;
        background-position-x: 0%;background-position-y: 0%;
        background-repeat-x: ;background-repeat-y: ;background-size: auto;box-sizing: border-box;
        color: rgb(64, 64, 64);display: block;font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;font-size: 14.4px;font-weight: 400;height: 173px;line-height: 24px;margin-bottom: 24px;padding-bottom: 12px;
        padding-left: 12px;padding-right: 12px;padding-top: 12px;
        text-size-adjust: 100%;width: 368.812px;-webkit-font-smoothing: antialiased;">
<p style="background-attachment: scroll;background-clip: border-box;
        background-color: rgb(26, 188, 156);background-image: none;
        background-origin: padding-box;background-position-x: 0%;
        background-position-y: 0%;background-repeat-x: ;background-repeat-y: ;background-size: auto;box-sizing: border-box;color: rgb(255, 255, 255);display: block;
        font-family: Lato, proxima-nova, "Helvetica Neue", Arial, sans-serif;font-size: 16px;font-style: normal;font-weight: 700;height: 29px;line-height: 16px;margin-bottom: 12px;
        margin-left: -12px;margin-right: -12px;margin-top: -12px;
        padding-bottom: 6px;padding-left: 12px;padding-right: 12px;
        padding-top: 6px;text-size-adjust: 100%;
        width: 368.812px;-webkit-font-smoothing: antialiased;
        -webkit-margin-after: 12px;-webkit-margin-before: -12px;-webkit-margin-end: -12px;-webkit-margin-start: -12px;"> Important message</p>
<p>Text</p>
</div>
