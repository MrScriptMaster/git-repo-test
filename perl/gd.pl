#!/usr/bin/perl

use strict;
use GD;
use GD::Graph::Data;
use GD::Graph::lines;
use GD::Graph::area;
use GD::Graph::linespoints;
use GD::Graph::points;
use GD::Graph::bars;
use GD::Graph::pie;

################################################################################
## Принципы рисования с помощью GD
## ===============================
## (низкоуровневое рисование)
##  1. Создать изображение - объект класса GD::Image, передав конструктору
##     размер.
##  2. Определить таблицу цветов с помощью метода GD::Image::colorAllocate(r,g,b).
##  3. Рисование фигур, текста и прочего с помощью методов класса.
##  4. Также вы можете рисовать полигонами.
##  5. После того, как рисование закончено, записать поток в бинарном виде в
##     файл.
##
## Рисование диаграмм
## ==================
## Рисовать диаграммы можно через следующие модули:
##  - GD::Graph::bars - столбчатая диаграмма (вертикальная).
##  - GD::Graph::hbars - столбчатая диаграмма (горизонтальная).
##  - GD::Graph::pie - круговая диаграмма.
##  - GD::Graph::lines - линейчатая диаграмма.
##  - GD::Graph::area - линейчатая диаграмма с заливкой.
##  - GD::Graph::linespoints - линейчатая диаграмма с узловыми точками.
##  - GD::Graph::points - точечная диаграмма.
##
################################################################################
=encoding UTF-8
=over

=item create_png($image, $filename)

Создает файл из GD объекта $image, сохраняя его в файл $fname.

=back

=cut

sub create_png($$) {
    my($image,$fname) = @_;
    open (my $file, ">$fname.png") or return 0;
    binmode($file);
    print $file $image->png();
    close $file;
    return 1;
}

=over

=item str2unicode($string)

Конвертирует входящую строку $string в unicode-строку.

=back

=cut

sub str2unicode($) {
    my $ustr = '';
    for my $chr (split //, shift) {
        if (ord($chr) > 127) {
            $ustr .= "&#" . (unpack("U", $chr)+848) . ";";
        } else {
            $ustr .= $chr;
        }
    }
    return $ustr;
}

sub draw_line_diagram() {

    my @x_values =   ('1', '2', '3', '4', '5', '6');
    my @y_values_1 = (10, 30, 40, 15, 80, 32);
    my @y_values_2 = (20, 25, 44, 18, 66, 12);
    my @data = (\@x_values, \@y_values_1, \@y_values_2);

    my %plot_info = (
        title         => 'Line diagram',
        x_label       => 'x values',
        y_label       => 'y values',
        dclrs         => [('green','blue')],
        x_label_skip  => 1,
        y_label_skip  => 1,
        y_tick_number => 20,
        boxclr        => 'white',
    );

    # Линейные диаграмма
    my $graph = GD::Graph::lines->new(500, 350);
    $graph->set(%plot_info) or warn $graph->error;
    $graph->set_legend_font('GD::gdMediumNormalFont');
    $graph->set_legend('line 1', 'line 2');
    my $plot = $graph->plot(\@data) or die $graph->error;
    create_png($plot, "line_graph");
    $graph = undef;

    # Линейная диаграмма с заливкой
    $graph = GD::Graph::area->new(500, 350);
    $graph->set(%plot_info) or warn $graph->error;
    $graph->set_legend_font('GD::gdMediumNormalFont');
    $graph->set_legend('line 1', 'line 2');
    my $plot = $graph->plot(\@data) or die $graph->error;
    create_png($plot, "line_graph_1");
    $graph = undef;

    # Линейная диаграмма с точками
    $graph = GD::Graph::linespoints->new(500, 350);
    $graph->set(%plot_info) or warn $graph->error;
    $graph->set_legend_font('GD::gdMediumNormalFont');
    $graph->set_legend('line 1', 'line 2');
    my $plot = $graph->plot(\@data) or die $graph->error;
    create_png($plot, "line_graph_2");
    $graph = undef;

    # Точечная диаграмма
    $graph = GD::Graph::points->new(500, 350);
    $graph->set(%plot_info) or warn $graph->error;
    $graph->set_legend_font('GD::gdMediumNormalFont');
    $graph->set_legend('line 1', 'line 2');
    my $plot = $graph->plot(\@data) or die $graph->error;
    create_png($plot, "line_graph_3");
    $graph = undef;
}

sub draw_bar() {
    my @months = ('Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec');
    my @vals = (100,150,200,250,50,34,99,12,300,250,110,123);
    my @vals_1 = (25,250,300,150,60,50,200,120,350,240,10,74);
    my @data = (\@months, \@vals, \@vals_1);

    my %plot_info = (
        title         => 'Diagram',
        x_label       => 'Months',
        y_label       => 'Values',
        dclrs         => [('green', 'blue')],
        x_label_skip  => 1,
        y_label_skip  => 1,
        y_tick_number => 8,
    );

    my $g = GD::Graph::bars->new(500, 350);
    $g->set(%plot_info) or warn $g->error;
    $g->set_legend('Bar 1', 'Bar 2');
    my $plot = $g->plot(\@data) or die $g->error;
    create_png($plot, "bar_diagram");
    $g = undef;

    # Данные можно передавать через объект класса GD::Graph::Data.
    my $data = GD::Graph::Data->new(
        [
            ['1','2','3','4','5','6','7','8','9'],
            [1  ,2  ,5  ,6  ,3  ,1.5,1  ,3  ,4  ],
        ]
    ) or die GD::Graph::Data->error;
    $g = GD::Graph::bars->new(500,350);
    # Пояснения к некоторым параметрам
    $g->set(
        x_label          => 'X label',
        y_label          => 'Y label',
        title            => 'Simple chart',
        y_max_value      => 7,   # максимальное значние по вертикальной оси
        y_tick_number    => 8,   # число рисок по вертикальной оси
        y_label_skip     => 3,   # определяет через сколько рисок писать рядом с ними значение по оси
        x_labels_vertical => 1,  # надписи рисок для горизонтальной оси будут писаться вертикально
        bar_spacing      => 5,   # делает промежутки между столбиками
        shadow_depth     => 4,   # рисует тень для столбиков. Положительные числа задают источник освещения слева, отрицательные - справа.
        shadowclr        => 'dred', # задает цвет тени
        transparent      => 0,   # делает задник области рисования непрозрачным (по умолчанию он прозрачный)
    ) or die GD::Graph::Data->error;
    $plot = $g->plot($data) or die $g->error;
    create_png($plot, "bar_diagram_1");
    $g = undef;
}

sub draw_pie() {
    my @chunk  = ('item 1', 'item 2', 'item 3');
    my @values = (45, 25, 30);
    my @data = (\@chunk, \@values);

    my $g = GD::Graph::pie->new(500, 350);
    $g->set(
        title        => "Pie",
        transparent  => 1,
        "3d"         => 1,
        dclrs        => [('green', 'blue', 'yellow')],
    );

    my $pie = $g->plot(\@data) or die $g->error;
    create_png($pie, "pie");
    $pie = undef;
}

draw_line_diagram();
draw_bar();
draw_pie();

1;

################################################################################
## Описание атрибутов:
##   * b_margin|t_margin|l_margin|r_margin => (integer)
##      Соответственно отступ снизу, сверху, слева и справа от границы изображения
##      до границы бокса, в котором рисуется график.
##
##   * width|height => (integer)
##      Размеры изображения, включая отступы.
##
##   * transparent => (boolean)
##      Включает/отключает прозрачность фона. По умолчанию true. Если прозрачность      
##      отключена, то применяется цвет bgclr (по умолчанию белый).
##
##   * interlaced => (boolean)
##      По умолчанию true. Указывает как генерировать PNG: чересчерточно или нет.
## 
##   * bgclr|fgclr|boxcolor|textclr|labelclr|axislabelclr|legendclr|valuesclr|accentclr|shadowclr => [colorname]
##      Задают цвета для различных частей изображения: соответственно фона, переднего плана, бокса,
##      текста, подписей, подписей осей, легенды, подписей для значений, основной цвет,
##      цвет затенения. Для некоторых цветов есть заранее определенные имена. Также
##      можно создавать свои цвета.
## 
##   * dclrs => \@colornames
##      Задает цвета для графиков, которые рисуются в одной области. По умолчанию
##      массив задан так:
##      ['lred', 'lgreen', 'lblue', 'lyellow', 'lpurple', 'cyan', 'lorange']
## 
##   * show_values => (boolean|array_ref)
##      Указывает на то, нужно ли подписывать точки на графике. Если будет передан
##      массив, то подписи будут браться из него.
## 
##   * values_vertical => (boolean)
##      Указывает направление написания подписей: горизонтально (по умолчанию),
##      вертикально.
## 
##   * values_space => (integer)
##      Если show_values=true, то определяет расстояние по вертикали между точкой
##      и надписью. По умолчнию это 4 пикселя.
## 
##   * values_format => (string|func_ref)
##      Если show_values=true, то определяет формат, по которому нужно отобразить
##      подпись. Формат определяется в sprintf-стиле, либо может быть ссылка на
##      функцию, которая возвращает этот формат.
## 
## Атрибуты для графиков с осями:
##   * box_axis => (boolean)
##      Если true, то рисуется стандартная коробка с двумя осями.
##
##   * two_axis => (boolean)
##      Если true, то график будет отрисован по двум осям: первый график по левой,
##      второй - по правой.
## 
##   * zero_axis|zero_axis_only => (boolean)
##      Для zero_axis=true ось всегда будет рисоваться от y=0. Если zero_axis_only=true,
##      то будет нарисована только ось y, а все значения x будут нанесены на нее.
##      Если оба флага false, то значения будут нанесены на нижнюю границу области.
##      По умолчанию оба флага true.
## https://www.oreilly.com/library/view/perl-graphics-programming/9781449358280/ch04s04.html
################################################################################
