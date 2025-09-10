#include "app.h"
#include "wi-fi.h"
#include "content.h"



static Content json;
static Content httpHeader;
static Content htmlHeader1;
static Content htmlTitle;
static Content htmlHeader2;
static Content footer;


/* VIEW */

View::View( App * app, Simba * simba ) : _app(app), _simba(simba) {

    httpHeader.SetNext( &htmlHeader1 );

    htmlHeader1.Init("<!DOCTYPE html>"
        "<html lang=\"es\">"
        "<head>"
        "  <meta charset=\"UTF-8\">"
        "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "  <title>", 
        &htmlTitle);

    htmlTitle.SetNext(&htmlHeader2);

    htmlHeader2.Init("</title>"
        "  <link rel=\"icon\" href='data:image/svg+xml,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 16 16\"><text y=\"14\" font-size=\"14\">🔧</text></svg>' />"
        "  <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css\" rel=\"stylesheet\">"
        "</head>"
        "<body class=\"bg-light\">",nullptr);

    footer.Init("  <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js\"></script>"
        "</body>", nullptr);

} 

void LayoutRender::RenderHeader(char * title){

    htmlTitle.SetSection(title);

}

void LayoutRender::RenderHttpHeader(int statusCode, ContentType contentType) {
    static char header[256];  

    const char* statusText;
    const char* contentTypeText;

    switch (statusCode) {
        case 200: statusText = "OK"; break;
        case 201: statusText = "Created"; break;
        case 404: statusText = "Not Found"; break;
        case 500: statusText = "Internal Server Error"; break;
        default:  statusText = "Unknown"; break;
    }

    switch (contentType) {
        case ContentType::APPLICATION_JSON:
            contentTypeText = "application/json"; break;
        case ContentType::TEXT_HTML:
            contentTypeText = "text/html"; break; 
    }


    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n"
        "\r\n",
        statusCode, statusText, contentTypeText, httpHeader.next->TotalLength());

    httpHeader.SetSection(header);
}




void LayoutRender::RenderFooter(){
    
}

void LayoutRender::SetJson(Content * json) {
    httpHeader.SetNext(json);
}

void LayoutRender::SetInnerContent(Content * innerContent) {
    httpHeader.SetNext(&htmlHeader1);
    htmlHeader2.SetNext(innerContent);
}

void LayoutRender::SetEnd(Content * penultimateContent) {
    penultimateContent->SetNext(&footer);
}

size_t LayoutRender::TotalLength() {
    return httpHeader.TotalLength();
}

Content * LayoutRender::GetRenderedLayout() {
    printf("ADENTRO DE GET RENDERED LAYOUT\n");
    printf("%ld\r\n",httpHeader.length);
    printf("%s\r\n", httpHeader.section);
    return &httpHeader;
}
