<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ taglib prefix="fmt" uri="http://java.sun.com/jsp/jstl/fmt" %>

<!DOCTYPE html>
<html lang="zh-Hant">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>智慧記帳管理系統 </title>
    <link rel="stylesheet" href="網頁程設.css">
    <style>
        .jsp-badge {
            display: inline-block;
            background-color: #28a745;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 12px;
            margin-left: 10px;
        }
        
        .success-message {
            background-color: #d4edda;
            border: 1px solid #c3e6cb;
            color: #155724;
            padding: 15px;
            margin: 15px;
            border-radius: 5px;
            display: none;
            animation: slideDown 0.3s ease;
        }
        
        .success-message.show {
            display: block;
        }
        
        @keyframes slideDown {
            from {
                opacity: 0;
                transform: translateY(-10px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
    </style>
</head>
<body>

    <!-- ========================= -->
    <!-- Header -->
    <!-- ========================= -->
    <header>
        <nav class="navbar">
            <div class="logo">
                智慧記帳管理系統 
                <span class="jsp-badge">JSP 互動版</span>
            </div>
        </nav>
    </header>

    <!-- ========================= -->
    <!-- Success Message -->
    <!-- ========================= -->
    <c:if test="${not empty message}">
        <div class="success-message show">
            ${message}
        </div>
    </c:if>

    <!-- ========================= -->
    <!-- Dashboard -->
    <!-- ========================= -->
    <section class="dashboard">
        <div class="card income-card">
            <h2>本月收入</h2>
            <p id="incomeTotal">
                $<c:set var="income" value="0"/>
                <c:forEach var="record" items="${sessionScope.records}">
                    <c:if test="${record.type == '收入'}">
                        <c:set var="income" value="${income + record.amount}"/>
                    </c:if>
                </c:forEach>
                ${income}
            </p>
        </div>

        <div class="card expense-card">
            <h2>本月支出</h2>
            <p id="expenseTotal">
                $<c:set var="expense" value="0"/>
                <c:forEach var="record" items="${sessionScope.records}">
                    <c:if test="${record.type == '支出'}">
                        <c:set var="expense" value="${expense + record.amount}"/>
                    </c:if>
                </c:forEach>
                ${expense}
            </p>
        </div>

        <div class="card remain-card">
            <h2>淨利或淨損</h2>
            <p id="profitTotal">$${income - expense}</p>
        </div>
    </section>

    <!-- ========================= -->
    <!-- Add Expense Button -->
    <!-- ========================= -->
    <section class="button-section">
        <button id="toggleFormBtn" type="button">
            + 新增記帳
        </button>
    </section>

    <!-- ========================= -->
    <!-- Form Section -->
    <!-- ========================= -->
    <section id="form-section" class="form-section" style="display: none;">
        <form id="expenseForm" action="ExpenseServlet" method="POST">
            <input type="hidden" name="action" value="add">

            <h2>新增記帳資料 (JSP 互動表單)</h2>

            <!-- 收入 / 支出 -->
            <div class="form-group">
                <label>類型：</label>
                <input type="radio" name="type" value="收入" required>
                收入
                <input type="radio" name="type" value="支出" checked required>
                支出
            </div>

            <!-- 金額 -->
            <div class="form-group">
                <label>金額：</label>
                <input type="number" id="amount" name="amount" min="0" required>
            </div>

            <!-- 日期 -->
            <div class="form-group">
                <label>日期：</label>
                <input type="date" id="date" name="date" max="9999-12-31" required>
            </div>

            <!-- 類別 -->
            <div class="form-group">
                <label>類別：</label>
                <select id="category" name="category" required>
                    <option>飲食</option>
                    <option>交通</option>
                    <option>娛樂</option>
                    <option>購物</option>
                    <option>學習</option>
                    <option>其他</option>
                </select>
            </div>

            <!-- 備註 -->
            <div class="form-group">
                <label>備註：</label>
                <textarea id="note" name="note"></textarea>
            </div>

            <!-- 按鈕 -->
            <div class="form-buttons">
                <button type="submit">
                    新增資料 (送到 Servlet)
                </button>
                <button type="reset">
                    清空
                </button>
            </div>

        </form>
    </section>

    <!-- ========================= -->
    <!-- History Section -->
    <!-- ========================= -->
    <section class="history-section">
        <h2>歷史紀錄 (JSP 動態顯示)</h2>

        <c:if test="${not empty sessionScope.records}">
            <table>
                <thead>
                    <tr>
                        <th>日期</th>
                        <th>類型</th>
                        <th>類別</th>
                        <th>金額</th>
                        <th>備註</th>
                        <th>操作</th>
                    </tr>
                </thead>
                <tbody>
                    <c:forEach var="record" items="${sessionScope.records}" varStatus="status">
                        <tr>
                            <td>${record.date}</td>
                            <td>${record.type}</td>
                            <td>${record.category}</td>
                            <td>$${record.amount}</td>
                            <td>${record.note}</td>
                            <td>
                                <form action="ExpenseServlet" method="POST" style="display: inline;">
                                    <input type="hidden" name="action" value="delete">
                                    <input type="hidden" name="index" value="${status.index}">
                                    <button type="submit" class="deleteBtn" onclick="return confirm('確認刪除此記錄？')">
                                        刪除
                                    </button>
                                </form>
                            </td>
                        </tr>
                    </c:forEach>
                </tbody>
            </table>
            <p style="text-align: center; margin-top: 15px; color: #666;">
                共 ${sessionScope.records.size()} 筆記錄
            </p>
        </c:if>

        <c:if test="${empty sessionScope.records}">
            <p style="text-align: center; color: #999; padding: 30px;">
                📝 暫無記帳資料，點擊「+ 新增記帳」開始記帳吧！
            </p>
        </c:if>
    </section>

    <script>
        // 設置今天的日期
        let today = new Date();
        let yyyy = today.getFullYear();
        let mm = String(today.getMonth() + 1).padStart(2, '0');
        let dd = String(today.getDate()).padStart(2, '0');
        
        document.getElementById('date').value = yyyy + '-' + mm + '-' + dd;

        // 表單開關
        document.getElementById('toggleFormBtn').addEventListener('click', function(e) {
            e.preventDefault();
            let formSection = document.getElementById('form-section');
            if (formSection.style.display === 'none') {
                formSection.style.display = 'block';
            } else {
                formSection.style.display = 'none';
            }
        });

        // 隱藏成功訊息
        let message = document.querySelector('.success-message');
        if (message && message.classList.contains('show')) {
            setTimeout(function() {
                message.classList.remove('show');
            }, 3000);
        }
    </script>

</body>
</html>
