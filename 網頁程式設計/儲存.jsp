<%@ page contentType="application/json;charset=UTF-8" %>

<%
String type = request.getParameter("type");
String amount = request.getParameter("amount");
String date = request.getParameter("date");
String category = request.getParameter("category");
String note = request.getParameter("note");

application.setAttribute("type",type);
application.setAttribute("amount",amount);
application.setAttribute("date",date);
application.setAttribute("category",category);
application.setAttribute("note",note);

out.print("{\"success\":true}");
%>