<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ taglib prefix="fmt" uri="http://java.sun.com/jsp/jstl/fmt" %>

<!DOCTYPE html>
<html lang="zh-Hant">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>記帳資料 - JSP 版本</title>
    <link rel="stylesheet" href="../網頁程設.css">
    <style>
        .jsp-info {
            background-color: #d4edda;
            border: 1px solid #c3e6cb;
            color: #155724;
            padding: 15px;
            margin: 15px;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <header>
        <nav class="navbar">
            <div class="logo">
                智慧記帳管理系統 - JSP 版本
            </div>
        </nav>
    </header>

    <section class="dashboard">
        <div class="jsp-info">
            <h3>✓ JSP 已成功整合</h3>
            <p>此頁面由 Java Servlet 和 JSP 動態生成</p>
        </div>
    </section>

    <section style="margin: 30px; background-color: white; padding: 20px; border-radius: 10px;">
        <h2>記帳資料列表</h2>
        
        <c:if test="${not empty expenseList}">
            <table border="1" style="width: 100%; border-collapse: collapse;">
                <thead>
                    <tr style="background-color: #333; color: white;">
                        <th style="padding: 10px;">日期</th>
                        <th style="padding: 10px;">類型</th>
                        <th style="padding: 10px;">類別</th>
                        <th style="padding: 10px;">金額</th>
                        <th style="padding: 10px;">備註</th>
                    </tr>
                </thead>
                <tbody>
                    <c:forEach var="record" items="${expenseList}">
                        <tr>
                            <td style="padding: 10px;">${record.date}</td>
                            <td style="padding: 10px;">${record.type}</td>
                            <td style="padding: 10px;">${record.category}</td>
                            <td style="padding: 10px;">$${record.amount}</td>
                            <td style="padding: 10px;">${record.note}</td>
                        </tr>
                    </c:forEach>
                </tbody>
            </table>
        </c:if>

        <c:if test="${empty expenseList}">
            <p style="color: #666; text-align: center; padding: 20px;">
                暫無記帳資料
            </p>
        </c:if>
    </section>

    <section style="margin: 30px; text-align: center;">
        <a href="../網頁程設.html" style="display: inline-block; padding: 10px 20px; background-color: #007bff; color: white; text-decoration: none; border-radius: 5px;">
            返回主頁面
        </a>
    </section>

</body>
</html>
