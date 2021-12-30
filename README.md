# MinesweeperCS161

(File .exe nằm trong folder build)

Giải thích nhanh code:
Các screen: LOGO, TITLE, DIFFICULTY, CUSTOM, GAMEPLAY, WINLOSE, SETTINGS

- currentScreen=LOGO
- Tạo icon cho window game
- Tạo nhạc cho game
- Nhập dữ liệu từ file savefile.dat trong folder build
- Trong lúc game chạy
  - Lấy data nhạc trong lúc còn chạy game loop, bấm P để chơi/dừng nhạc
  - LOGO: Đợi 2s rồi chuyển qua TITLE
  - TITLE: Tạo các nút New Game, Continue, Settings, Save & Exit Game
    - New Game: chuyển qua DIFFICULTY
    - Continue: Nếu ko cần tạo mới, thì chuyển qua GAMEPLAY
    - Settings: chuyên qua SETTINGS
    - Save & Exit Game: lưu dữ liệu vào file và đóng window
  - DIFFICULTY: Tạo các nút Easy, Intermediate, Hard, Custom và Back
    - Easy: 8 cột, 8 dòng, 10 bomb, tạo mới game, chuyển qua GAMEPLAY
    - Intermediate: 16 cột, 12dòng, 50 bomb, tạo mới game, chuyển qua GAMEPLAY
    - Hard: 24 cột, 16 dòng, 100 bomb, tạo mới game, chuyển qua GAMEPLAY
    - Custom: chuyển qua CUSTOM
    - Back: chuyển qua TITLE
  - CUSTOM: 3 thanh kéo cho Cột, dòng, số bomb, 1 nút Create
    - width: chọn giá trị cột
    - height: chọn giá trị dòng
    - bombs: chọn giá trị số bomb
    - CREATE: chuyển qua GAMEPLAY
  - GAMEPLAY: Ma trận game chính, 3 thông số gồm score, timer và bombs, 2 nút BACK và CHECK
    - Cứ 1s từ khi tạo mới game thì timer++;
    - Khi tạo mới game:
      - Làm sạch 2 ma trận grid.
      - Random các số bomb định trước trên grid sao cho vừa hết
      - Thông qua grid để cập nhập số bomb gần nó là bao nhiêu
      - Reset các thông số score, timer, ngưng đổi mới
    - Khi click chuột trái
      - Nếu ko phải bomb, score++
      - Nếu ô đó là ô trống, BFS các ô xung quanh
      - Nếu là bomb, chuyển qua WINLOSE, thua game, cập nhập score
    - Khi click chuốt phải
      - Nếu ô chưa bấm thì đặt >, và ngược lại
    - BACK: chuyển qua TITLE
    - CHECK: kiểm tra nếu như đặt ? trên tất cả các bomb chưa, nếu rồi thì chuyển qua WINLOSE, thắng game, cặp nhập score
  - WINLOSE: 1 nút BACK, 2 thông số score và hiscore
    - BACK: chuyển qua TITLE, cho phép đổi mới game
    - hiscore = max(hiscore, score)
  - SETTINGS: 2 nút RESET DATA và BACK, các thông số cần theo dõi
    - RESET DATA: xoá sạch các thông số
    - BACK: chuyển qua TITLE

Để tiện quản lý, các screen được quản lý bởi struct Program và các thông tin (điểm số, thời gian, bảng chơi, ...) được quản lý bởi struct User, struct Table:

* struct User:
- phương thức create(): khởi tạo kích thước bảng chơi.
- phương thức timerCount(): quản lý thời gian chơi.
- phương thức generate(): vẽ bảng chơi và cài đặt các thông số về mức ban đầu.
- phương thức BFS(): chạy thuật toán loang trên bảng chơi để mở rộng các ô trống.

* struct Program:
- Gồm tọa độ click chuột mousey, mousex.
- Thuộc tính screen cho biết màn hình hiện tại của game, được thay đổi trong quá trình thao tác.

* struct Table:
- hbomb: chiều cao bảng chơi.
- wbomb: chiều rộng bảng chơi.
- vbomb: số lượng bomb.
- flagged[][[]: đánh dấu ô đã được gắn cờ hay chưa.
- opened[][]: đánh dấu ô đã được mở hay chưa.
- grid[][]: bảng chứa thông tin các ô (số lượng bomb hoặc bomb)