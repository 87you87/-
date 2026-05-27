// script.js

$(document).ready(function(){

    // =========================
    // 表單開關
    // =========================

    $("#toggleFormBtn").click(function(){

        $("#form-section").slideToggle();

    });

    // =========================
    // 新增記帳資料
    // =========================

    $("#expenseForm").submit(function(e){

        e.preventDefault();

        // 取得資料
        let type = $("input[name='type']:checked").val();

        let amount = $("#amount").val();

        let date = $("#date").val();

        let category = $("#category").val();

        let note = $("#note").val();

        // 新增到表格
        let row = `
            <tr>

                <td>${date}</td>

                <td>${type}</td>

                <td>${category}</td>

                <td>${amount}</td>

                <td>${note}</td>

                <td>
                    <button class="deleteBtn">
                        刪除
                    </button>
                </td>

            </tr>
        `;

        $("#historyTable").append(row);

        // 清空表單
        $("#expenseForm")[0].reset();

    });

    // =========================
    // 刪除資料
    // =========================

    $(document).on("click",".deleteBtn",function(){

        $(this).closest("tr").remove();

    });

    // =========================
    // 圓餅圖
    // =========================

    const pieCtx = document.getElementById('pieChart');

    new Chart(pieCtx, {

        type: 'pie',

        data: {

            labels: ['飲食', '交通', '娛樂', '購物'],

            datasets: [{
                data: [500,300,200,400]
            }]
        }

    });

    // =========================
    // 長條圖
    // =========================

    const barCtx = document.getElementById('barChart');

    new Chart(barCtx, {

        type: 'bar',

        data: {

            labels: ['1月','2月','3月','4月'],

            datasets: [{
                label: '每月支出',
                data: [5000,7000,6500,8000]
            }]
        }

    });

});