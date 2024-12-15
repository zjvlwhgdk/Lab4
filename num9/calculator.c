#include <gtk/gtk.h>
#include <stdio.h>

typedef struct {
    GtkWidget *entry1, *entry2, *result_label;
} Widgets;

// 계산 함수 (사칙연산 처리)
void calculate(GtkWidget *widget, gpointer data) {
    Widgets *w = (Widgets *)data;
    const gchar *entry_text1 = gtk_entry_get_text(GTK_ENTRY(w->entry1));
    const gchar *entry_text2 = gtk_entry_get_text(GTK_ENTRY(w->entry2));

    double num1 = atof(entry_text1);  // 첫 번째 숫자
    double num2 = atof(entry_text2);  // 두 번째 숫자
    double result = 0;
    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    if (g_strcmp0(button_label, "덧셈") == 0) {
        result = num1 + num2;
    } else if (g_strcmp0(button_label, "뺄셈") == 0) {
        result = num1 - num2;
    } else if (g_strcmp0(button_label, "곱셈") == 0) {
        result = num1 * num2;
    } else if (g_strcmp0(button_label, "나눗셈") == 0) {
        if (num2 != 0) {
            result = num1 / num2;
        } else {
            gtk_label_set_text(GTK_LABEL(w->result_label), "0으로 나눌 수 없습니다!");
            return;
        }
    }

    // 결과 레이블 갱신
    char result_text[50];
    snprintf(result_text, sizeof(result_text), "결과: %.2f", result);
    gtk_label_set_text(GTK_LABEL(w->result_label), result_text);

    // 입력 필드 초기화
    gtk_entry_set_text(GTK_ENTRY(w->entry1), "");
    gtk_entry_set_text(GTK_ENTRY(w->entry2), "");
}

// 윈도우 종료 시 호출되는 함수
void quit(GtkWidget *window, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *vbox, *hbox1, *hbox2;
    GtkWidget *button_add, *button_subtract, *button_multiply, *button_divide;
    GtkWidget *label1, *label2, *result_label;
    Widgets *w = g_slice_new(Widgets); // 사용자 정의 위젯 구조체 할당

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "간단한 계산기");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);  // 수직 박스

    // 첫 번째 숫자 입력
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);  // 수평 박스
    label1 = gtk_label_new("숫자 1:");
    w->entry1 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), w->entry1, TRUE, TRUE, 5);

    // 두 번째 숫자 입력
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);  // 수평 박스
    label2 = gtk_label_new("숫자 2:");
    w->entry2 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), w->entry2, TRUE, TRUE, 5);

    // 결과 표시 레이블
    result_label = gtk_label_new("결과:");
    w->result_label = result_label;

    // 사칙연산 버튼들
    button_add = gtk_button_new_with_label("덧셈");
    button_subtract = gtk_button_new_with_label("뺄셈");
    button_multiply = gtk_button_new_with_label("곱셈");
    button_divide = gtk_button_new_with_label("나눗셈");

    // 버튼에 이벤트 연결
    g_signal_connect(button_add, "clicked", G_CALLBACK(calculate), w);
    g_signal_connect(button_subtract, "clicked", G_CALLBACK(calculate), w);
    g_signal_connect(button_multiply, "clicked", G_CALLBACK(calculate), w);
    g_signal_connect(button_divide, "clicked", G_CALLBACK(calculate), w);

    // 레이아웃 구성
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_add, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_subtract, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_multiply, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_divide, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), result_label, FALSE, FALSE, 5);

    // 윈도우에 모든 위젯 추가
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
